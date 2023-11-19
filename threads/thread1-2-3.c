#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    printf("tid %ld\n", gettid());
    //pthread_detach(pthread_self());
    return NULL;
}

int main() {
    pthread_t tid;
    int err;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    while(1) {
        err = pthread_create(&tid, &attr, mythread, NULL);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
        //pthread_join(tid, NULL);
    }
}