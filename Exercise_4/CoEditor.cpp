#include "CoEditor.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

CoEditor::CoEditor(BoundedBuffer& input, BoundedBuffer& shared, const std::string& editorType)
    : inputBuffer(input), sharedBuffer(shared), type(editorType) {}

void CoEditor::edit() {
    while (true) {

        if (inputBuffer.isEmpty()) {
            continue; // If the buffer is empty, passing iteration
        }

        char* rawMessage = inputBuffer.remove();
        if (rawMessage == nullptr) {
            continue; // If returns null, passing iteration
        }

        std::string message = std::string(rawMessage);
        free(rawMessage); // Free dynamically allocated memory

        // If the received message is "DONE", pass it directly to the shared buffer and exit
        if (strcmp(message.c_str(), "DONE") == 0) {
            sharedBuffer.insert(const_cast<char*>(message.c_str())); 
            break; 
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        sharedBuffer.insert(const_cast<char*>(message.c_str()));
    }
}