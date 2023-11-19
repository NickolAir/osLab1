#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

//#include "queue.h"

/*typedef struct _QueueNode {
    int val;
    struct _QueueNode *next;
} qnode_t;

typedef struct _Queue {
    qnode_t *first;
    qnode_t *last;

    int count;
    int max_count;

    long add_attempts;
    long get_attempts;
    long add_count;
    long get_count;

    pthread_spinlock_t lock;
} queue_t;*/

queue_t* queue_init(int max_count) {
    int err;

    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    q->first = NULL;
    q->last = NULL;
    q->max_count = max_count;
    q->count = 0;

    err = pthread_spin_init(&q->lock, PTHREAD_PROCESS_SHARED);
    if (err) {
        printf("queue_init: pthread_spin_init() failed: %s\n", strerror(err));
        abort();
    }

    return q;
}

void queue_destroy(queue_t *q) {
    pthread_spin_destroy(&q->lock);

    if (q == NULL) {
        return;
    }

    while (q->first != NULL) {
        qnode_t *tmp = q->first;
        q->first = q->first->next;
        free(tmp);
    }

    free(q);
}

int queue_add(queue_t *q, int val) {
    pthread_spin_lock(&q->lock);

    q->add_attempts++;
    printf("Запись: попытки %ld\n", q->add_attempts);

    if (q->count >= q->max_count) {
        pthread_spin_unlock(&q->lock);
        return 0;
    }

    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new) {
        pthread_spin_unlock(&q->lock);
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
    q->add_count++;
    printf("Запись: успешно %ld\n", q->add_count);


    pthread_spin_unlock(&q->lock);

    return 1;
}

int queue_get(queue_t *q, int *val) {
    pthread_spin_lock(&q->lock);

    q->get_attempts++;
    printf("Чтение: попытки %ld\n", q->get_attempts);


    if (q->count == 0) {
        pthread_spin_unlock(&q->lock);
        return 0;
    }

    qnode_t *tmp = q->first;

    *val = tmp->val;
    q->first = q->first->next;

    free(tmp);
    q->count--;
    q->get_count++;
    printf("Чтение: успешно %ld\n", q->get_count);


    pthread_spin_unlock(&q->lock);

    return 1;
}

void queue_print_stats(queue_t *q) {
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
           q->count,
           q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
           q->add_count, q->get_count, q->add_count -q->get_count);
}