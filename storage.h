#ifndef OSLAB1_STR_LIST_H
#define OSLAB1_STR_LIST_H

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
} Storage;



Storage* storage_init(int max_count);
void queue_destroy(queue_t *q);
int queue_add(queue_t *q, int val);
int queue_get(queue_t *q, int *val);
void queue_print_stats(queue_t *q);

#endif //OSLAB1_STR_LIST_H