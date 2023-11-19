#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

struct ThreadResult {
    int value;
    char *string;
};

void *mythread(void *arg) {
    struct ThreadResult *res = (struct ThreadResult*)arg;
    res->value = 42;
    res->string = "Hello world";
    printf("%d %s\n", res->value, res->string);
    return NULL;
}

int main() {
    pthread_t tid;
    int err;
    struct ThreadResult res;

    err = pthread_create(&tid, NULL, mythread, &res);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    } else {
        printf("Thread created!\n");
    }
    sleep(5);
    return 0;
}