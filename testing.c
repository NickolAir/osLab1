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

#include "queue.h"

int main() {
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Number of available CPU cores: %d\n", num_cores);

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    for (int i = 0; i < num_cores + 100; i++) {
        CPU_SET(i, &cpu_set);
    }

    // Теперь все доступные процессоры добавлены в множество cpu_set.

    return 0;
}
