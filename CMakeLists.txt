cmake_minimum_required(VERSION 3.26)
project(osLab1 C)

set(CMAKE_C_STANDARD 11)

add_executable(osLab1 testing.c
        queue_mutex.c
        queue_sem.c
        queue_sync.c
        queue_cond.c
)