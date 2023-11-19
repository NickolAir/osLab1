#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "queue.h"
#include <semaphore.h>

/*typedef struct _QueueNode {
    int val;
    struct _QueueNode *next;
} qnode_t;

typedef struct _Queue {
    qnode_t *first;
    qnode_t *last;

    sem_t empty;
    sem_t full;

    long add_attempts;
    long get_attempts;
    long add_count;
    long get_count;

    pthread_mutex_t mutex;
    int count;
    int max_count;
} queue_t;*/

queue_t* queue_init(int max_count) {
    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    q->first = NULL;
    q->last = NULL;
    q->max_count = max_count;
    q->count = 0;

    if (pthread_mutex_init(&q->mutex, NULL) != 0) {
        printf("Failed to initialize mutex\n");
        abort();
    }

    if (sem_init(&q->empty, 0, max_count) != 0) {
        printf("Failed to initialize empty semaphore\n");
        abort();
    }

    if (sem_init(&q->full, 0, 0) != 0) {
        printf("Failed to initialize full semaphore\n");
        abort();
    }

    return q;
}

void queue_destroy(queue_t *q) {
    sem_destroy(&q->empty);
    sem_destroy(&q->full);
    pthread_mutex_destroy(&q->mutex);
    free(q);
}

int queue_add(queue_t *q, int val) {
    sem_wait(&q->empty);
    q->add_attempts++;
    printf("Запись: попытки %ld\n", q->add_attempts);

    pthread_mutex_lock(&q->mutex);

    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new) {
        printf("Cannot allocate memory for new node\n");
        abort();
    }

    new->val = val;
    new->next = NULL;

    if (!q->first)
        q->first = q->last = new;
    else {
        q->last->next = new;
        q->last = q->last->next;
    }

    q->count++;
    q->add_attempts++;

    printf("Запись: успешно %ld\n", q->add_attempts);

    pthread_mutex_unlock(&q->mutex);

    sem_post(&q->full);

    return 1;
}

int queue_get(queue_t *q, int *val) {
    sem_wait(&q->full);
    q->get_attempts++;
    printf("Чтение: попытки %ld\n", q->get_attempts);

    pthread_mutex_lock(&q->mutex);

    qnode_t *tmp = q->first;

    *val = tmp->val;
    q->first = q->first->next;

    free(tmp);
    q->count--;
    q->get_count++;

    printf("Чтение: успешно %ld\n", q->get_count);

    pthread_mutex_unlock(&q->mutex);

    sem_post(&q->empty);

    return 1;
}

void queue_print_stats(queue_t *q) {
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
           q->count,
           q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
           q->add_count, q->get_count, q->add_count -q->get_count);
}