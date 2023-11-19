#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void *sigint_handler(void *arg) {
    sigset_t set;
    int sig;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    while(1) {
        sigwait(&set, &sig);

        printf("Получен сигнал SIGINT.\n");
    }
}

void *sigquit_handler(void *arg) {
    sigset_t set;
    int sig;

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);

    while (1) {
        sigwait(&set, &sig);

        printf("Получен сигнал SIGQUIT.\n");
    }
}

int main() {
    pthread_t sigint_thread, sigquit_thread;

    printf("pid %d\n", getpid());

    sigset_t all_signals;
    sigfillset(&all_signals);
    pthread_sigmask(SIG_BLOCK, &all_signals, NULL);

    pthread_create(&sigint_thread, NULL, sigint_handler, NULL);
    pthread_create(&sigquit_thread, NULL, sigquit_handler, NULL);

    while (1){
        sleep(1);
       /* const int kill_err2 = pthread_kill(sigint_thread, SIGINT);
        if (kill_err2) {
            fprintf(stderr, "main: pthread_kill(3) failed: %s\n", strerror(kill_err2));
        }*/

        /*const int kill_err3 = pthread_kill(sigquit_thread, SIGQUIT);
        if (kill_err3) {
            fprintf(stderr, "main: pthread_kill(3) failed: %s\n", strerror(kill_err3));
        }*/
    }
}