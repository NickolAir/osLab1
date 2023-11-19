#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    //pthread_setcancelstate(PTHREAD_CANCEL_DEFERRED, NULL);
    int i = 0;
    while(1) {
        i++;
        pthread_testcancel();
    }
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    sleep(5);
    pthread_cancel(tid);
}