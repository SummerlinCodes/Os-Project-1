#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shared_memory.h"

using namespace std;

SharedTable* sharedTable = nullptr;
pthread_t    consumerThread;
sem_t*       mutex_sem   = nullptr;
sem_t*       empty_sem   = nullptr;
sem_t*       full_sem    = nullptr;

// Consume an item by printing it
void consumeItem(int item) {
    cout << "Consumer: Consumed item " << item << endl;
}

// Signal handler flips the shared flag
typedef void (*sighandler_t)(int);
void sigintHandler(int) {
    cout << "\nConsumer: Terminating..." << endl;
    sharedTable->isRunning = false;
}

// Consumer thread function
void* consumerFunc(void*) {
    while (sharedTable->isRunning) {
        // Wait for an item and mutex
        sem_wait(full_sem);
        sem_wait(mutex_sem);

        int item = sharedTable->items[sharedTable->out];
        cout << "Consumer: Got item " << item
             << " from position " << sharedTable->out << endl;
        sharedTable->out = (sharedTable->out + 1) % TABLE_SIZE;

        sem_post(mutex_sem);
        sem_post(empty_sem);

        consumeItem(item);
        sleep(rand() % 5 + 1);
    }
    return nullptr;
}

int main() {
    // Set up signal handler
    signal(SIGINT, (sighandler_t)sigintHandler);
    // Seed random number generator
    srand(time(nullptr));

    // Open shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Map shared memory
    sharedTable = (SharedTable*)mmap(
        NULL, sizeof(SharedTable), PROT_READ | PROT_WRITE,
        MAP_SHARED, shm_fd, 0
    );
    if (sharedTable == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    sharedTable->isRunning = true;  // Set running flag to true

    // Open semaphores
    mutex_sem = sem_open(SEM_MUTEX, 0);
    empty_sem = sem_open(SEM_EMPTY, 0);
    full_sem  = sem_open(SEM_FULL,  0);
    if (mutex_sem==SEM_FAILED || empty_sem==SEM_FAILED || full_sem==SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Create consumer thread
    if (pthread_create(&consumerThread, nullptr, consumerFunc, nullptr) != 0) {
        perror("pthread_create");
        exit(1);
    }

    // Wait for consumer thread to finish
    pthread_join(consumerThread, nullptr);

    // Cleanup
    sem_close(mutex_sem);
    sem_close(empty_sem);
    sem_close(full_sem);
    munmap(sharedTable, sizeof(SharedTable));

    return 0;
}
