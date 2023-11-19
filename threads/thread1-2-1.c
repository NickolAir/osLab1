#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <malloc.h>
#include <string.h>

void *mythread(void *arg) {
    static char s[] = "sdfsdf";
    return s;
}

int main() {
    pthread_t tid;
    int err;
    char* arg = "";
    char** arg_ptr = &arg;

    err = pthread_create(&tid, NULL, mythread, arg_ptr);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    } else {
        printf("Thread created!\n");
        // join блокирует поток, пока не будет выполнена функция mythread
        pthread_join(tid, &arg);
        printf("%s", arg);
    }
    return 0;
}