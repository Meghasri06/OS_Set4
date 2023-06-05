/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define SHARED_MEMORY_KEY 12345
#define NUM_VALUES 100

typedef struct {
    int max;
    int min;
    double avg;
} Result;

void generateRandomNumbers(int* numbers) {
    srand(time(NULL));
    for (int i = 0; i < NUM_VALUES; i++) {
        numbers[i] = rand() % 1000;
    }
}

int main() {
    // Create shared memory segment
    int shmid = shmget(SHARED_MEMORY_KEY, sizeof(int) * NUM_VALUES, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory segment
    int* sharedMemory = (int*)shmat(shmid, NULL, 0);
    if (sharedMemory == (int*)-1) {
        perror("shmat");
        exit(1);
    }

    pid_t childPid;
    Result* result;

    // Create child process
    childPid = fork();

    if (childPid < 0) {
        perror("fork");
        exit(1);
    } else if (childPid == 0) {
        // Child process

        // Wait for the parent to write numbers into shared memory
        while (sharedMemory[0] == 0) {
            usleep(1000);
        }

        // Compute maximum, minimum, and average
        int max = sharedMemory[0];
        int min = sharedMemory[0];
        double sum = 0;

        for (int i = 0; i < NUM_VALUES; i++) {
            int value = sharedMemory[i];
            if (value > max) {
                max = value;
            }
            if (value < min) {
                min = value;
            }
            sum += value;
        }

        // Compute the average
        double avg = sum / NUM_VALUES;

        // Write the result into shared memory
        result = (Result*)(sharedMemory + NUM_VALUES);
        result->max = max;
        result->min = min;
        result->avg = avg;

        // Detach shared memory
        if (shmdt(sharedMemory) == -1) {
            perror("shmdt");
            exit(1);
        }

        exit(0);
    } else {
        // Parent process

        // Generate random numbers
        int numbers[NUM_VALUES];
        generateRandomNumbers(numbers);

        // Write numbers into shared memory
        for (int i = 0; i < NUM_VALUES; i++) {
            sharedMemory[i] = numbers[i];
        }

        // Wait for the child to finish computation
        wait(NULL);

        // Read the result from shared memory
        result = (Result*)(sharedMemory + NUM_VALUES);
        int max = result->max;
        int min = result->min;
        double avg = result->avg;

        // Display the result
        printf("Maximum: %d\n", max);
        printf("Minimum: %d\n", min);
        printf("Average: %.2f\n", avg);

        // Detach shared memory
        if (shmdt(sharedMemory) == -1) {
            perror("shmdt");
            exit(1);
        }

        // Remove shared memory segment
        if (shmctl(shmid, IPC_RMID, 0) == -1) {
            perror("shmctl");
            exit(1);
        }
    }

    return 0;
}
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHM_SIZE 100 * sizeof(int)

typedef struct {
    int numbers[100];
    int max;
    int min;
    float average;
} SharedData;

void parentProcess(SharedData *data) {
    printf("Parent process is running...\n");

    // Generate random numbers
    for (int i = 0; i < 100; i++) {
        data->numbers[i] = rand() % 1000;
    }

    // Wait for the child process to finish computing
    while (data->max == 0 || data->min == 0 || data->average == 0) {
        sleep(1);
    }

    // Print the results
    printf("Max: %d\n", data->max);
    printf("Min: %d\n", data->min);
    printf("Average: %.2f\n", data->average);
}

void childProcess(SharedData *data) {
    printf("Child process is running...\n");

    // Calculate maximum, minimum, and average
    int max = data->numbers[0];
    int min = data->numbers[0];
    int sum = data->numbers[0];

    for (int i = 1; i < 100; i++) {
        if (data->numbers[i] > max) {
            max = data->numbers[i];
        }
        if (data->numbers[i] < min) {
            min = data->numbers[i];
        }
        sum += data->numbers[i];
    }

    data->max = max;
    data->min = min;
    data->average = (float)sum / 100;
}

int main() {
    // Create shared memory segment
    int shmId = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shmId == -1) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory segment
    SharedData *data = (SharedData *)shmat(shmId, NULL, 0);
    if (data == (SharedData *)(-1)) {
        perror("shmat");
        exit(1);
    }

    // Initialize shared memory data
    data->max = 0;
    data->min = 0;
    data->average = 0;

    // Create child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        childProcess(data);

        // Detach shared memory segment from child
        if (shmdt(data) == -1) {
            perror("shmdt");
            exit(1);
        }
    } else {
        // Parent process
        parentProcess(data);

        // Wait for child process to exit
         wait(NULL);

        // Detach and remove shared memory segment
        if (shmdt(data) == -1) {
            perror("shmdt");
            exit(1);
        }
        if (shmctl(shmId, IPC_RMID, NULL) == -1) {
            perror("shmctl");
            exit(1);
        }
    }

    return 0;
}
