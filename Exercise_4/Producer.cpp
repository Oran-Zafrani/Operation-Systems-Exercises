#include "Producer.h"

// Constructor
Producer::Producer(int id, int totalProducts, int bufferSize)
    : id(id), totalProducts(totalProducts), buffer(bufferSize) {}

// Helper function to generate random categories
std::string Producer::getRandomCategory() {
    static const std::vector<std::string> categories = {"WEATHER", "SPORTS", "NEWS"};
    static std::random_device rd;  // Seed for random number generator
    static std::mt19937 gen(rd()); // Mersenne Twister random number generator
    static std::uniform_int_distribution<> dist(0, categories.size() - 1);

    return categories[dist(gen)];
}

// Method to produce strings to the buffer
void Producer::produce() {
    for (int i = 0; i < totalProducts; ++i) {
        std::string category = getRandomCategory();
        std::string product = "producer " + std::to_string(id) + " " + category + " " + std::to_string(i);

        // Insert the product into the buffer
        buffer.insert(const_cast<char*>(product.c_str()));
        std::cout << "**DEBUG** Producer " << id << " produced: " << product << std::endl;
    }
    buffer.insert(const_cast<char*>("DONE"));
    std::cout << "**DEBUG** Producer " << id << " finished producing" << std::endl;

}

// Method to get the bounded buffer
BoundedBuffer& Producer::getBuffer() {
    return buffer;
}