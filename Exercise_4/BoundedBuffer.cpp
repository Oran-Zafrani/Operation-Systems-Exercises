#include "BoundedBuffer.h"

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstring> // For strdup
#include <queue>
#include <condition_variable>
#include <mutex>

// BinarySemaphore method implementations
BinarySemaphore::BinarySemaphore(bool initial) : signal(initial) {}

void BinarySemaphore::wait() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return signal; });
    signal = false; // Reset the signal after waiting
}

void BinarySemaphore::signalSem() {
    std::unique_lock<std::mutex> lock(mtx);
    signal = true;
    cv.notify_one();
}

// CountingSemaphore method implementations
CountingSemaphore::CountingSemaphore(int initial) : S2(true), S1(true), count(initial) {}

void CountingSemaphore::wait() {
    S2.wait();
    S1.wait();
    --count;
    if (count > 0) {
        S2.signalSem();
    }
    S1.signalSem();
}

void CountingSemaphore::signal() {
    S1.wait();
    ++count;
    if (count >= 1) {
        S2.signalSem();
    }
    S1.signalSem();
}

// BoundedBuffer method implementations
BoundedBuffer::BoundedBuffer(int size)
    : size(size), in(0), out(0), count(0), mutex(true), empty(size), full(0) {
    buffer.resize(size, nullptr);
}

void BoundedBuffer::insert(char* s) {
    empty.wait();       // Wait for an empty slot
    mutex.wait();       // Enter critical section

    // Insert the string into the buffer
    buffer[in] = strdup(s); // Duplicate the string to store it
    in = (in + 1) % size;   // Move to the next index
    ++count;

    mutex.signalSem(); // Exit critical section
    full.signal();     // Signal that a slot is full
}

char* BoundedBuffer::remove() {
    full.wait();       // Wait for a filled slot
    mutex.wait();      // Enter critical section

    // Remove the string from the buffer
    char* s = buffer[out];
    buffer[out] = nullptr; // Clear the slot
    out = (out + 1) % size; // Move to the next index
    --count;

    mutex.signalSem(); // Exit critical section
    empty.signal();    // Signal that a slot is empty

    return s;          // Return the removed string
}

BoundedBuffer::~BoundedBuffer() {
    for (auto& s : buffer) {
        if (s) {
            free(s);
        }
    }
}