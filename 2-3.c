#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#include "storage.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

void fill_string(char *str, int order) {
    for (int i = 0; i < order % 100; i++) {
        str[i] = 'A' + i % 26;
    }
}

void set_cpu(int n) {
    int err;
    cpu_set_t cpuset;
    pthread_t tid = pthread_self();

    CPU_ZERO(&cpuset);
    CPU_SET(n, &cpuset);

    err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
    if (err) {
        printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
        return;
    }

    printf("set_cpu: set cpu %d\n", n);
}

void *incrLength(void *arg) {
    storage_t *s = (storage_t *)arg;

    set_cpu(0);
    while (1) {

    }
}

void *decrLength(void *arg) {
    storage_t *s = (storage_t *)arg;

    set_cpu(1);
    while (1) {

    }
}

void *sameLength(void *arg) {
    storage_t *s = (storage_t *)arg;

    set_cpu(2);
    while (1) {

    }
}

int main() {
    pthread_t tid;
    storage_t *s;
    int err;

    printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

    s = storage_init(100);

    for (int i = 0; i < 100; i++) {
        char tmp[100];
        fill_string(tmp, i);

        int ok = storage_add(s, tmp);

        printf("ok %d: add value %d\n", ok, i);
        printf("string: %s\n", tmp);

        storage_print_stats(s);
    }

    err = pthread_create(&tid, NULL, incrLength, s);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid, NULL, decrLength, s);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid, NULL, sameLength, s);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_exit(NULL);

    return 0;
}