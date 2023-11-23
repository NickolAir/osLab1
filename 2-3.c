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
#include <time.h>

#include "storage.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

#define MAX_COUNT 10

int iterations_th1 = 0;
int iterations_th2 = 0;
int iterations_th3 = 0;
int swapped = 0;
int attempts = 0;

void fill_string(char *str, int order) {
    int i;
    for (i = 0; i < order % 100; i++) {
        str[i] = 'A' + i % 26;
    }
    str[i] = '\0';
}

int get_length(Node *node) {
    return strlen(node->value);
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

void swap(Node *headNode, Node *NodeA, Node *NodeB, Node *tailNode) {
    headNode->next = NodeB;
    NodeB->next = NodeA;
    NodeA->next = tailNode;
}

void print_nodes(storage_t *s) {
    Node *cur = s->first;
    int i = 1;
    while(cur != s->last) {
        printf("Node %d: %s\n", i, cur->value);
        i++;
        cur = cur->next;
    }
    printf("Node %d: %s\n", i, cur->value);
    printf("\n");
}

void *incrLength(void *arg) {
    srand(time(NULL));
    storage_t *s = (storage_t *)arg;
    Node *cur = s->first;
    int i = 0;

    set_cpu(0);
    while (cur->next != s->last) {
        if (get_length(cur) < get_length(cur->next)){
            i++;
        }
        cur = cur->next;
    }

    iterations_th1 += i;
    printf("iterations in 1st thread %d\n", i);

    cur = s->first;
    while(1) {
        pthread_spin_lock(&s->lock);
        if (cur == s->last) {
            cur = s->first;
            printf("thread 1\n");
            print_nodes(s);
        }
        usleep(150000);
        int condition = rand() % 2;
        if (condition == 1 && cur->next != NULL && cur->next->next != NULL && cur->next->next->next != NULL) {
            swap(cur, cur->next, cur->next->next, cur->next->next->next);
            attempts++;
            swapped++;
        } else {
            attempts++;
        }
        cur = cur->next;
        pthread_spin_unlock(&s->lock);
    }
}

void *decrLength(void *arg) {
    srand(time(NULL));
    storage_t *s = (storage_t *)arg;
    Node *cur = s->first;
    int i = 0;

    set_cpu(1);
    while (cur->next != s->last) {
        if (get_length(cur) > get_length(cur->next)){
            i++;
        }
        cur = cur->next;
    }

    iterations_th2 += i;
    printf("iterations in 2nd thread %d\n", i);

    cur = s->first;
    while(1) {
        pthread_spin_lock(&s->lock);
        if (cur == s->last) {
            cur = s->first;
            printf("thread 2\n");
            print_nodes(s);
        }
        usleep(150000);
        int condition = rand() % 2;
        if (condition == 1 && cur->next != NULL && cur->next->next != NULL && cur->next->next->next != NULL) {
            swap(cur, cur->next, cur->next->next, cur->next->next->next);
            attempts++;
            swapped++;
        } else {
            attempts++;
        }
        cur = cur->next;
        pthread_spin_unlock(&s->lock);
    }
}

void *sameLength(void *arg) {
    srand(time(NULL));
    storage_t *s = (storage_t *)arg;
    Node *cur = s->first;
    int i = 0;

    set_cpu(2);
    while (cur->next != s->last) {
        if (get_length(cur) == get_length(cur->next)){
            i++;
        }
        cur = cur->next;
    }

    iterations_th3 += i;
    printf("iterations in 3rd thread %d\n", i);

    cur = s->first;
    while(1) {
        pthread_spin_lock(&s->lock);
        if (cur == s->last) {
            cur = s->first;
            printf("thread 3\n");
            print_nodes(s);
        }
        usleep(150000);
        int condition = rand() % 2;
        if (condition == 1 && cur->next != NULL && cur->next->next != NULL && cur->next->next->next != NULL) {
            swap(cur, cur->next, cur->next->next, cur->next->next->next);
            attempts++;
            swapped++;
        } else {
            attempts++;
        }
        cur = cur->next;
        pthread_spin_unlock(&s->lock);
    }
}

int main() {
    srand(time(NULL));
    pthread_t tid;
    storage_t *s;
    int err;

    printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

    s = storage_init(MAX_COUNT);

    for (int i = 0; i < MAX_COUNT; i++) {
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

    storage_destroy(s);

    return 0;
}