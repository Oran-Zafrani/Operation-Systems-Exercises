#ifndef PRODUCER_H
#define PRODUCER_H

#include <string>
#include <vector>
#include <random>
#include <iostream>
#include "BoundedBuffer.h"

class Producer {
private:
    int id;                  // Producer ID
    int totalProducts;       // Number of products to produce
    BoundedBuffer buffer;    // BoundedBuffer instance

    // Helper function to generate random categories
    std::string getRandomCategory();

public:
    // Constructor
    Producer(int id, int totalProducts, int bufferSize);

    // Method to produce strings to the buffer
    void produce();

    // Method to get the BoundedBuffer
    BoundedBuffer& getBuffer();
};

#endif // PRODUCER_H