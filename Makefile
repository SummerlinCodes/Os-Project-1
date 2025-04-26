# Os-Project-1

## Description
# Producer and Consumer problem implementation for Kent State University Operating Systems class.
#
# This project demonstrates the classic producer-consumer problem using:
# - Shared memory between two processes
# - Semaphores for synchronization
# - Threads in both producer and consumer programs
# - A table that can hold two items at a time

## Usage
# Two ways to compile and run:
#
# 1. **Using Makefile**:
#    - Type `make build` to compile
#    - Type `make run` to execute both programs
#    - This assumes you have make installed.
#
# 2. **Manual way**:
#    - Type `g++ consumer.cpp -o consumer -lrt -pthread`
#    - Type `g++ producer.cpp -o producer -lrt -pthread`
#    - To run the program, type: `./producer & ./consumer &`

## Implementation Details
# - The producer generates items and puts them on the table
# - The consumer picks up items from the table
# - The table can only hold two items at a time
# - When the table is full, the producer waits
# - When the table is empty, the consumer waits
# - Semaphores are used for synchronization
# - Shared memory is used for the "table"

CC       = g++
CFLAGS   = -std=c++17 -pthread -lrt
TARGET1  = producer
TARGET2  = consumer
SRC1     = producer.cpp
SRC2     = consumer.cpp

all: build

build: $(TARGET1) $(TARGET2)

$(TARGET1): $(SRC1)
	$(CC) $(SRC1) -o $(TARGET1) $(CFLAGS)

$(TARGET2): $(SRC2)
	$(CC) $(SRC2) -o $(TARGET2) $(CFLAGS)

run: build
	@echo "Starting (CTRL+C to stop):"
	@echo "Starting producer in background…"
	@./producer &
	@sleep 1
	@echo "Starting consumer…"
	@./consumer

clean:
	rm -f $(TARGET1) $(TARGET2)

# Cleanup script to remove any leftover shared memory or semaphores
cleanup:
	ipcs -m | grep 0x | awk '{print $$2}' | xargs -n1 ipcrm -m 2>/dev/null || true
	ipcs -s | grep 0x | awk '{print $$2}' | xargs -n1 ipcrm -s 2>/dev/null || true
	rm -f /dev/shm/producer_consumer_shm
	rm -f /dev/shm/sem.mutex_sem
	rm -f /dev/shm/sem.empty_sem
	rm -f /dev/shm/sem.full_sem