#ifndef OSLAB1_STORAGE_H
#define OSLAB1_STORAGE_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdatomic.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct _Node {
    char value[100];
    struct _Node*  next;
    pthread_mutex_t sync;
} Node;

typedef struct _Storage {
    Node *first;
    Node *last;

    pthread_t monitor_tid;

    long add_attempts;
    long get_attempts;
    long add_count;
    long get_count;

    pthread_mutex_t mutex;
    int count;
    int max_count;
} storage_t;

void *monitor(void *arg);
storage_t* storage_init(int max_count);
void storage_destroy(storage_t *s);
int storage_add(storage_t *s, char *val);
int storage_get(storage_t *s, char **val);
void storage_print_stats(storage_t *s);

#endif //OSLAB1_STORAGE_H