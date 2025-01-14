#include "Producer.h"

Producer::Producer(int id, int totalProducts, int bufferSize)
    : id(id), totalProducts(totalProducts), buffer(bufferSize) {}

// Helper function to generate random categories
std::string Producer::getRandomCategory() {
    static const std::vector<std::string> categories = {"WEATHER", "SPORTS", "NEWS"};
    static std::random_device rd;  
    static std::mt19937 gen(rd()); 
    static std::uniform_int_distribution<> dist(0, categories.size() - 1);

    return categories[dist(gen)];
}

// Method to produce strings to the buffer
void Producer::produce() {
    int weatherCount = 0, sportsCount = 0, newsCount = 0;
    for (int i = 0; i < totalProducts; ++i) {
        std::string category = getRandomCategory();
        std::string product = "";

        if (category == "WEATHER") {
            product = "producer " + std::to_string(id) + " " + category + " " + std::to_string(weatherCount);
            weatherCount++;
        } else if (category == "SPORTS") {
            product = "producer " + std::to_string(id) + " " + category + " " + std::to_string(sportsCount);
            sportsCount++;
        } else {
            product = "producer " + std::to_string(id) + " " + category + " " + std::to_string(newsCount);
            newsCount++;
        }

        // Insert the product into the buffer
        buffer.insert(const_cast<char*>(product.c_str()));
    }
    buffer.insert(const_cast<char*>("DONE"));

}

BoundedBuffer& Producer::getBuffer() {
    return buffer;
}