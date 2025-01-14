#ifndef BOUNDED_BUFFER_H
#define BOUNDED_BUFFER_H

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

// Binary Semaphore Class
class BinarySemaphore {
private:
    std::mutex mtx;
    std::condition_variable cv;
    bool signal;

public:
    // Constructor
    BinarySemaphore(bool initial = false);

    // Wait operation
    void wait();

    // Signal operation
    void signalSem();
};

// Counting Semaphore Class
class CountingSemaphore {
private:
    BinarySemaphore S2;
    BinarySemaphore S1;
    int count;

public:
    // Constructor
    CountingSemaphore(int initial);

    // Wait operation
    void wait();

    // Signal operation
    void signal();
};

// Bounded Buffer Class
class BoundedBuffer {
private:
    std::vector<char*> buffer;    // Circular buffer to store strings
    int size;                     // Maximum size of the buffer
    int in;                       // Index for inserting an element
    int out;                      // Index for removing an element
    int count;                    // Current number of items in the buffer

    BinarySemaphore mutex;        // Mutex semaphore for mutual exclusion
    CountingSemaphore empty;      // Semaphore to track empty slots
    CountingSemaphore full;       // Semaphore to track filled slots

public:
    BoundedBuffer(int size);

    void insert(char* s);

    char* remove();

    bool isEmpty();
    
    // Destructor
    ~BoundedBuffer();
};

#endif 