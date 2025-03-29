#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include "shared_memory.h"

using namespace std;

// Global variables
SharedTable* sharedTable = NULL;
sem_t* mutex = NULL;
sem_t* empty = NULL;
sem_t* full = NULL;

// Function to consume an item
void consumeItem(int item) {
    cout << "Consumer: Consumed item " << item << endl;
}

// Function for consumer thread
void* consumer(void* arg) {
    while (true) {
        // Wait for item to be available
        sem_wait(full);
        
        // Wait for mutex
        sem_wait(mutex);
        
        // Critical section - get item from table
        int item = sharedTable->items[sharedTable->out];
        cout << "Consumer: Got item " << item << " from position " << sharedTable->out << endl;
        sharedTable->out = (sharedTable->out + 1) % TABLE_SIZE;
        
        // Release mutex
        sem_post(mutex);
        
        // Signal that table has an empty slot
        sem_post(empty);
        
        // Consume the item
        consumeItem(item);
        
        // Sleep for random time (1-5 seconds)
        sleep(rand() % 5 + 1);
    }
    
    return NULL;
}

int main() {
    // Seed random number generator
    srand(time(NULL));
    
    // Open shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    // Map shared memory
    sharedTable = (SharedTable*)mmap(NULL, sizeof(SharedTable), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (sharedTable == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    // Open semaphores
    mutex = sem_open(SEM_MUTEX, 0);
    empty = sem_open(SEM_EMPTY, 0);
    full = sem_open(SEM_FULL, 0);
    
    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }
    
    // Create consumer thread
    pthread_t consumerThread;
    if (pthread_create(&consumerThread, NULL, consumer, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    
    // Wait for consumer thread to finish (which it never will in this implementation)
    pthread_join(consumerThread, NULL);
    
    // Clean up (this code will never be reached in this implementation)
    sem_close(mutex);
    sem_close(empty);
    sem_close(full);
    munmap(sharedTable, sizeof(SharedTable));
    
    return 0;
}