#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <malloc.h>

void *mythread(void *arg) {
    printf("tid %ld\n", pthread_self());
    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    while(1) {
        err = pthread_create(&tid, NULL, mythread, NULL);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        } else {
            printf("Thread created!\n");
            // join блокирует поток, пока не будет выполнена функция mythread
            pthread_join(tid, NULL);
        }
    }
}