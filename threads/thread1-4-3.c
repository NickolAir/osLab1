#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

void cleanup(void *arg) {
    free(arg);
}

void *mythread(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    char *string = malloc(sizeof(char) * 12);
    strcpy(string, "Hello world!");

        pthread_cleanup_push(cleanup, string);

        while (1) {
            printf("%s\n", string);
            pthread_testcancel();
        }

        pthread_cleanup_pop(1);
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    sleep(4);
    pthread_cancel(tid);
}