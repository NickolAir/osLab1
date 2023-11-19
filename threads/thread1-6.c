#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ucontext.h>

#define PAGE 4096
#define STACK_SIZE PAGE * 8

typedef void *(*start_routine_t)(void *);

typedef struct {
    int mythread_id;
    start_routine_t start_routine;
    ucontext_t before_start_routine;
    void *arg;
    void *retval;
    volatile int joined;
    volatile int exited;
    volatile int cancelled;
} mythread_struct_t;

typedef mythread_struct_t *mythread_t;


int mythread_startup(void *arg) {
    mythread_struct_t *mythread = (mythread_struct_t *) arg;


    getcontext(&(mythread->before_start_routine));
    if(!mythread->cancelled)
        mythread->retval = mythread->start_routine(mythread);

    mythread->exited = 1;

    while (!mythread->joined) {
        usleep(1);
    }

    return 0;
}

int create_stack(void** stack, off_t size, int thread_num) {
    char stack_file[128];
    int stack_fd;
    snprintf(stack_file, sizeof(stack_file), "stack-%d", thread_num);
    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    int err = ftruncate(stack_fd, 0);
    err = ftruncate(stack_fd, size);
    *stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_STACK, stack_fd, 0);
    printf("stack address %p\n", stack);
    close(stack_fd);
    memset(*stack, 0x7f, size);
    return 0;
}

int mythread_create(mythread_t *mytid, start_routine_t start_routine, void *arg) {
    static int thread_num = 0;

    void *child_stack = NULL;
    int err = create_stack(&child_stack,STACK_SIZE, thread_num);
    if(err == -1){
        printf("mythread_create: failed to create stack");
        return -1;
    }
    mprotect(child_stack + PAGE, STACK_SIZE - PAGE, PROT_READ | PROT_WRITE);

    mythread_struct_t *mythread = (mythread_struct_t *) (child_stack + STACK_SIZE - sizeof(mythread_struct_t));
    printf("struct address %p\n", *mythread);
    mythread->mythread_id = thread_num;
    mythread->start_routine = start_routine;
    mythread->arg = arg;
    mythread->joined = 0;
    mythread->exited = 0;
    mythread->retval = NULL;
    mythread->cancelled = 0;

    thread_num++;

    child_stack = (void *) mythread;
    int child_pid = clone(mythread_startup, child_stack, CLONE_VM | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND | SIGCHLD, (void *) mythread);
    if (child_pid == -1) {
        printf("clone failed: %s \n", strerror(errno));
        return -1;
    }

    *mytid = mythread;

    return 0;
}

int mythread_join(mythread_t mytid, void **retval) {
    mythread_struct_t *mythread = mytid;
    while (!mythread->exited) {
        sleep(1);
    }
    *retval = mythread->retval;
    mythread->joined = 1;
    return 0;
}

void mythread_cancel(mythread_t mytid){
    mythread_struct_t *mythread = mytid;
    mythread->retval = "cancelled";
    mythread-> cancelled = 1;
}

void mythread_test_cancel(mythread_t mytid){
    mythread_struct_t *mythread = mytid;
    if(mythread->cancelled)
        setcontext(&(mythread->before_start_routine));
}

void *mythread(void *arg) {
    mythread_struct_t *mythread = (mythread_struct_t *) arg;
    char *str = (char *)mythread->arg;

    /*while(1) {
        sleep(1);
    }*/
    for (int i = 0; i < 5; ++i) {
        printf("hello: %s\n", str);
        sleep(1);
        mythread_test_cancel(mythread);
    }
    return "goodbye";
}

int main() {
    mythread_t mytid;
    void *retval;
    int err = mythread_create(&mytid, mythread, "hi");
    if(err == -1){
        printf("thread create failed\n");
    }

    sleep(2);
    //mythread_cancel(mytid);
    mythread_join(mytid, &retval);
    printf("%d thread returned '%s'\n", gettid(), (char *) retval);
    return 0;
}