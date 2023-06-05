#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define turn 5
#define NUM_READERS 2
#define NUM_WRITERS 2

sem_t mutex, writeMutex;
int readersCount = 0;
int sharedData = 0;

void *reader(void *arg) {
    int readerId = *(int *)arg;

    for(int i=0;i<turn;i++) {
        sem_wait(&mutex);
        readersCount++;
        if (readersCount == 1) {
            sem_wait(&writeMutex);
        }
        sem_post(&mutex);

        // Read from the shared data
        printf("Reader %d read: %d\n", readerId, sharedData);

        sem_wait(&mutex);
        readersCount--;
        if (readersCount == 0) {
            sem_post(&writeMutex);
        }
        sem_post(&mutex);

        // Sleep for a random interval before reading again
        usleep(rand() % 1000000);
    }
}

void *writer(void *arg) {
    int writerId = *(int *)arg;

     for(int i=0;i<turn;i++) {
        sem_wait(&writeMutex);

        // Write to the shared data
        sharedData++;
        printf("Writer %d wrote: %d\n", writerId, sharedData);

        sem_post(&writeMutex);

        // Sleep for a random interval before writing again
        usleep(rand() % 1000000);
    }
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    int readerIds[NUM_READERS], writerIds[NUM_WRITERS];
    int i;

    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&writeMutex, 0, 1);

    // Create reader threads
    for (i = 0; i < NUM_READERS; i++) {
        readerIds[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &readerIds[i]);
    }

    // Create writer threads
    for (i = 0; i < NUM_WRITERS; i++) {
        writerIds[i] = i + 1;
        pthread_create(&writers[i], NULL, writer, &writerIds[i]);
    }

    // Wait for reader threads to finish
    for (i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    // Wait for writer threads to finish
    for (i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&writeMutex);

    return 0;
}


