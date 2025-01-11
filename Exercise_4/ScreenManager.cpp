#include "ScreenManager.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

// Constructor
ScreenManager::ScreenManager(BoundedBuffer& shared)
    : sharedBuffer(shared), doneCount(0) {}

// Process messages from the shared buffer
void ScreenManager::process() {
    while (doneCount < 3) { // Wait for three "DONE" messages
        // Get a message from the shared buffer
        char* rawMessage = sharedBuffer.remove();
        if (rawMessage == nullptr) {
            continue; // Shared buffer might be empty, keep waiting for messages
        }

        std::string message = std::string(rawMessage);
        free(rawMessage); // Free dynamically allocated memory

        // Check for "DONE" message
        if (message == "DONE") {
            doneCount++; // Increment the "DONE" message count
        } else {
            // Simulate displaying the message on the screen
            std::cout << message << std::endl;
        }
    }

    // Exit after receiving three "DONE" messages
    std::cout << "DONE" << std::endl;
}