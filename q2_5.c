#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
//abccab
#define NUM_ITERATIONS 20
int turn =0;
int turnb=0;
sem_t sem_p1, sem_p2, sem_p3;

void *process_p1(void *threadId) {
    long tid = (long)threadId;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sem_wait(&sem_p1);

        printf("a");

        sem_post(&sem_p2);
    }

    pthread_exit(NULL);
}

void *process_p2(void *threadId) {
    long tid = (long)threadId;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sem_wait(&sem_p2);

        printf("b");
        turnb++;
        if(turnb %2 !=0){
            sem_post(&sem_p3);

        }
        else
    {
         sem_post(&sem_p1);
         printf("\n");
    }

    }

    pthread_exit(NULL);
}

void *process_p3(void *threadId) {
    long tid = (long)threadId;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sem_wait(&sem_p3);

        printf("c");
        turn++;
        if(turn%2 != 0)
            sem_post(&sem_p3);
        else
            sem_post(&sem_p1);
    }

    pthread_exit(NULL);
}

int main() {
     pthread_t thread_p1, thread_p2, thread_p3;

    // Initialize semaphores
    sem_init(&sem_p1, 0, 1);  // Semaphore for P1 (initially unlocked)
    sem_init(&sem_p2, 0, 0);  // Semaphore for P2 (initially locked)
    sem_init(&sem_p3, 0, 0);  // Semaphore for P3 (initially locked)

    // Create threads for P1, P2, and P3
    pthread_create(&thread_p1, NULL, process_p1, NULL);
    pthread_create(&thread_p2, NULL, process_p2, NULL);
    pthread_create(&thread_p3, NULL, process_p3, NULL);

    // Wait for the threads to finish
    pthread_join(thread_p1, NULL);
    pthread_join(thread_p2, NULL);
    pthread_join(thread_p3, NULL);

    // Destroy semaphores
    sem_destroy(&sem_p1);
    sem_destroy(&sem_p2);
    sem_destroy(&sem_p3);
}
