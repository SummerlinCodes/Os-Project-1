#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <iostream>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

// Shared memory name
#define SHM_NAME "/producer_consumer_shm"

// Semaphore names
#define SEM_MUTEX "/mutex_sem"
#define SEM_EMPTY "/empty_sem"
#define SEM_FULL "/full_sem"

// Table size (can hold 2 items)
#define TABLE_SIZE 2

// Structure for shared memory
typedef struct {
    int items[TABLE_SIZE];  // Array to hold items
    int in;                 // Index for producer to put item
    int out;                // Index for consumer to get item
} SharedTable;

#endif // SHARED_MEMORY_H