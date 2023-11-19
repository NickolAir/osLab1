#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global_var = 0;

void *mythread(void *arg) {
    int local_var = gettid();
    static int static_var = 20;
    const int const_var = 30;
    global_var++;
    local_var++;

    printf("mythread [%d %d %d %ld]: Hello from mythread!\n", getpid(), getppid(), gettid(), pthread_self());
    printf("local %d, global %d, static %d, const %d\n", local_var, global_var, static_var, const_var);
    printf("addr %p\n", &local_var);
    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    printf("main [%d %d %d %ld]: Hello from main!\n", getpid(), getppid(), gettid(), pthread_self());

    sleep(5);
    for (int i = 0; i < 5; ++i) {
        err = pthread_create(&tid, NULL, mythread, NULL);
        getchar();
        // join блокирует поток, пока не будет выполнена функция mythread
        pthread_join(tid, NULL);
    }

    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    return 0;
}