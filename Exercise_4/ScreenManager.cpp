#include "ScreenManager.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

ScreenManager::ScreenManager(BoundedBuffer& shared)
    : sharedBuffer(shared), doneCount(0) {}

void ScreenManager::process() {
    while (doneCount < 3) { // Wait for three "DONE" messages

        if (sharedBuffer.isEmpty()) {
            continue; // If the buffer is empty, pass the iteration
        }

        char* rawMessage = sharedBuffer.remove();
        if (rawMessage == nullptr) {
            continue; // Shared buffer might be empty, pass the iteration
        }

        std::string message = std::string(rawMessage);
        free(rawMessage); // Free dynamically allocated memory

        // Check for "DONE" message
        if (message == "DONE") {
            doneCount++; 
        } else {
            std::cout << message << std::endl;
        }
    }

    std::cout << "DONE" << std::endl;
}