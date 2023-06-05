#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t empty_slots;
sem_t full_slots;
sem_t mutex;

void *producer(void *arg) {
    int i;
    int item;

    for (i = 0; i < NUM_ITEMS; ++i) {
        item = rand() % 100;  // Generate a random item
        sem_wait(&empty_slots);
        sem_wait(&mutex);

        buffer[in] = item;
        printf("Producer produced item: %d\n", item);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&full_slots);
    }

    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int i;
    int item;

    for (i = 0; i < NUM_ITEMS; ++i) {
        sem_wait(&full_slots);
        sem_wait(&mutex);

        item = buffer[out];
        printf("Consumer consumed item: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&empty_slots);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t producerThread, consumerThread;

    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&full_slots, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_create(&producerThread, NULL, producer, NULL);
    pthread_create(&consumerThread, NULL, consumer, NULL);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    sem_destroy(&mutex);

    return 0;
}
