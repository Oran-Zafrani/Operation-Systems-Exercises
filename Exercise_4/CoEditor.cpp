#include "CoEditor.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

// Constructor
CoEditor::CoEditor(BoundedBuffer& input, BoundedBuffer& shared, const std::string& editorType)
    : inputBuffer(input), sharedBuffer(shared), type(editorType) {}

// Editing process
void CoEditor::edit() {
    while (true) {
        // Get a message from the input buffer
        char* rawMessage = inputBuffer.remove();
        if (rawMessage == nullptr) {
            continue; // If the buffer is empty, wait for a message
        }

        std::string message = std::string(rawMessage);
        free(rawMessage); // Free dynamically allocated memory

        // If the received message is "DONE", pass it directly to the shared buffer and exit
        if (message == "DONE") {
            sharedBuffer.insert(const_cast<char*>(message.c_str())); // Forward "DONE"
            break; // Exit the loop
        }

        // Simulate the editing process by sleeping for 0.1 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Forward the edited message to the shared buffer
        sharedBuffer.insert(const_cast<char*>(message.c_str()));

        // Debugging/Logging (optional)
        std::cout << "CoEditor (" << type << ") edited and forwarded message: " << message << std::endl;
    }
}