#define _GNU_SOURCE
#include <assert.h>
#include "storage.h"

void *monitor(void *arg) {
    storage_t *s = (storage_t *)arg;

    printf("monitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    /*while (1) {
        storage_print_stats(s);
        sleep(1);
    }*/

    return NULL;
}

storage_t* storage_init(int max_count) {
    int err;
    storage_t *s = malloc(sizeof(storage_t));
    if (!s) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    s->first = NULL;
    s->last = NULL;
    s->max_count = max_count;
    s->count = 0;

    s->add_attempts = s->get_attempts = 0;
    s->add_count = s->get_count = 0;

    err = pthread_spin_init(&s->lock, PTHREAD_PROCESS_SHARED);
    if (err) {
        printf("queue_init: pthread_spin_init() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_create(&s->monitor_tid, NULL, monitor, s);
    if (err) {
        printf("queue_init: pthread_create() failed: %s\n", strerror(err));
        abort();
    }
    return s;
}

void storage_destroy(storage_t *q) {
    pthread_spin_destroy(&q->lock);
    char **val;
    strcpy(*val, "-1");
    while(q->first != q->last) {
        storage_get(q, val);
    }
    storage_get(q, val);
}

int storage_add(storage_t *q, char* val) {
    q->add_attempts++;

    assert(q->count <= q->max_count);

    if (q->count == q->max_count)
        return 0;
    Node *new = malloc(sizeof(Node));
    if (!new) {
        printf("Cannot allocate memory for new node\n");
        abort();
    }

    strcpy(new->value, val);
    new->next = NULL;

    if (!q->first)
        q->first = q->last = new;
    else {
        q->last->next = new;
        q->last = q->last->next;
    }

    q->count++;
    q->add_count++;

    return 1;
}

int storage_get(storage_t *s, char **val) {
    s->get_attempts++;

    assert(s->count >= 0);

    if (s->count == 0)
        return 0;

    Node *tmp = s->first;

    strcpy(*val, tmp->value);
    s->first = s->first->next;

    free(tmp);
    s->count--;
    s->get_count++;

    return 1;
}

void storage_print_stats(storage_t *q) {
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
           q->count,
           q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
           q->add_count, q->get_count, q->add_count - q->get_count);
}