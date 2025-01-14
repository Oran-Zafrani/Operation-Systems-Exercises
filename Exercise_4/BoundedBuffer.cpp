#include "BoundedBuffer.h"

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstring> 
#include <queue>
#include <condition_variable>
#include <mutex>

BinarySemaphore::BinarySemaphore(bool initial) : signal(initial) {}

void BinarySemaphore::wait() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return signal; });
    signal = false; 
}

void BinarySemaphore::signalSem() {
    std::unique_lock<std::mutex> lock(mtx);
    signal = true;
    cv.notify_one();
}

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

BoundedBuffer::BoundedBuffer(int size)
    : size(size), in(0), out(0), count(0), mutex(true), empty(size), full(0) {
    buffer.resize(size, nullptr);
}

void BoundedBuffer::insert(char* s) {
    empty.wait();       
    mutex.wait();      

    // Insert the string into the buffer
    buffer[in] = strdup(s); // Duplicate the string to store it
    in = (in + 1) % size;   // Move to the next index
    ++count;

    mutex.signalSem(); 
    full.signal();     
}

char* BoundedBuffer::remove() {
    full.wait();       
    mutex.wait();      

    // Remove the string from the buffer
    char* s = buffer[out];
    buffer[out] = nullptr; // Clear the slot
    out = (out + 1) % size; // Move to the next index
    --count;

    mutex.signalSem(); 
    empty.signal();   

    return s;          
}

bool BoundedBuffer::isEmpty() {
    return count == 0;
}

BoundedBuffer::~BoundedBuffer() {
    for (auto& s : buffer) {
        if (s) {
            free(s);
        }
    }
}