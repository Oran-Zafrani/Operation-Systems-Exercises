#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <string>
#include "BoundedBuffer.h"
#include "Producer.h"
#include "Dispatcher.h"
#include "CoEditor.h"
#include "ScreenManager.h"

int main(int argc, char* argv[]) {
    // Check if the input file is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.txt>" << std::endl;
        return 1;
    }

    // Open the input file
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    // Variables to store data
    std::vector<Producer*> producers;          // Store producers
    std::vector<BoundedBuffer*> producerBuffers; // Store each producer's buffer (via getBuffer())
    int coEditorQueueSize = 0;                 // Size of the Co-Editor queue

    // Read and parse the input file
    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);

        // Check for PRODUCER line
        if (line.find("PRODUCER") != std::string::npos) {
            // Extract producer ID
            std::string producerId = line.substr(line.find("PRODUCER") + 9);

            // Read number of products
            std::getline(inputFile, line);
            int numberOfProducts = std::stoi(line);

            // Read queue size
            std::getline(inputFile, line);
            int queueSize = std::stoi(line.substr(line.find('=') + 1));

            // Create the producer
            Producer* producer = new Producer(std::stoi(producerId), numberOfProducts, queueSize);

            // Store the producer and its buffer
            producers.push_back(producer);
            BoundedBuffer* buffer = &producer->getBuffer();
            producerBuffers.push_back(buffer); // Use the getter to retrieve the buffer
        }
        // Check for Co-Editor queue size line
        else if (line.find("Co-Editor queue size") != std::string::npos) {
            coEditorQueueSize = std::stoi(line.substr(line.find('=') + 1));
        }
    }

    inputFile.close(); // Close the file

    // Ensure we have valid data
    if (producers.empty() || coEditorQueueSize <= 0) {
        std::cerr << "Error: Invalid input file format or missing data." << std::endl;
        return 1;
    }

    // Create the Dispatcher
    Dispatcher dispatcher(producerBuffers, coEditorQueueSize);

    // Create the shared buffer for the ScreenManager
    BoundedBuffer sharedBuffer(coEditorQueueSize);

    // Create CoEditors
    CoEditor sportsEditor(dispatcher.getSportsBuffer(), sharedBuffer, "sports");
    CoEditor newsEditor(dispatcher.getNewsBuffer(), sharedBuffer, "news");
    CoEditor weatherEditor(dispatcher.getWeatherBuffer(), sharedBuffer, "weather");

    // Create the ScreenManager
    ScreenManager screenManager(sharedBuffer);

    // Start Producer threads
    std::vector<std::thread> producerThreads;
    for (Producer* producer : producers) {
        producerThreads.emplace_back(&Producer::produce, producer);
    }

    // Start the Dispatcher thread
    std::thread dispatcherThread(&Dispatcher::dispatch, &dispatcher);

    // Start CoEditor threads
    std::thread sportsThread(&CoEditor::edit, &sportsEditor);
    std::thread newsThread(&CoEditor::edit, &newsEditor);
    std::thread weatherThread(&CoEditor::edit, &weatherEditor);

    // Start the ScreenManager thread
    std::thread screenManagerThread(&ScreenManager::process, &screenManager);

    // Wait for all threads to finish
    for (std::thread& thread : producerThreads) {
        thread.join();
    }
    dispatcherThread.join();
    sportsThread.join();
    newsThread.join();
    weatherThread.join();
    screenManagerThread.join();

    // Clean up dynamically allocated memory
    for (Producer* producer : producers) {
        delete producer;
    }

    return 0;
}