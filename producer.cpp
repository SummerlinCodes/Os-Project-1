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

// Function to produce an item
int produceItem() {
    static int item = 0;
    return ++item;
}

// Function for producer thread
void* producer(void* arg) {
    while (true) {
        // Produce an item
        int item = produceItem();
        
        // Wait for empty slot
        sem_wait(empty);
        
        // Wait for mutex
        sem_wait(mutex);
        
        // Critical section - put item on table
        sharedTable->items[sharedTable->in] = item;
        cout << "Producer: Produced item " << item << " at position " << sharedTable->in << endl;
        sharedTable->in = (sharedTable->in + 1) % TABLE_SIZE;
        
        // Release mutex
        sem_post(mutex);
        
        // Signal that table has an item
        sem_post(full);
        
        // Sleep for random time (1-3 seconds)
        sleep(rand() % 3 + 1);
    }
    
    return NULL;
}

int main() {
    // Seed random number generator
    srand(time(NULL));
    
    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    // Set size of shared memory
    if (ftruncate(shm_fd, sizeof(SharedTable)) == -1) {
        perror("ftruncate");
        exit(1);
    }
    
    // Map shared memory
    sharedTable = (SharedTable*)mmap(NULL, sizeof(SharedTable), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (sharedTable == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    // Initialize shared memory
    sharedTable->in = 0;
    sharedTable->out = 0;
    
    // Create semaphores
    mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);  // Mutex for critical section
    empty = sem_open(SEM_EMPTY, O_CREAT, 0666, TABLE_SIZE);  // Initially table is empty
    full = sem_open(SEM_FULL, O_CREAT, 0666, 0);  // Initially no items
    
    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }
    
    // Create producer thread
    pthread_t producerThread;
    if (pthread_create(&producerThread, NULL, producer, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    
    // Wait for producer thread to finish (which it never will in this implementation)
    pthread_join(producerThread, NULL);
    
    // Clean up (this code will never be reached in this implementation)
    sem_close(mutex);
    sem_close(empty);
    sem_close(full);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);
    munmap(sharedTable, sizeof(SharedTable));
    shm_unlink(SHM_NAME);
    
    return 0;
}