#include "Dispatcher.h"

// Constructor
Dispatcher::Dispatcher(const std::vector<BoundedBuffer*>& buffers, int dispatcherBufferSize)
    : producerBuffers(buffers),
      sportsBuffer(dispatcherBufferSize),   // Initialize bounded buffers for SPORTS
      newsBuffer(dispatcherBufferSize),     // Initialize bounded buffers for NEWS
      weatherBuffer(dispatcherBufferSize),  // Initialize bounded buffers for WEATHER
      producerDone(buffers.size(), false),  // Initialize all producers as "not done"
      allProducersDone(false) {}

// Helper function to sort messages into the appropriate dispatcher buffer
void Dispatcher::sortMessage(const std::string& message) {
    if (message.find("SPORTS") != std::string::npos) {
        sportsBuffer.insert(const_cast<char*>(message.c_str())); // Insert into SPORTS buffer
        std::cout << "**DEBUG** Dispatcher received " << message << " and inserted into sports buffer" << std::endl;
    } else if (message.find("NEWS") != std::string::npos) {
        newsBuffer.insert(const_cast<char*>(message.c_str())); // Insert into NEWS buffer
        std::cout << "**DEBUG** Dispatcher received " << message << " and inserted into news buffer" << std::endl;
    } else if (message.find("WEATHER") != std::string::npos) {
        weatherBuffer.insert(const_cast<char*>(message.c_str())); // Insert into WEATHER buffer
        std::cout << "**DEBUG** Dispatcher received " << message << " and inserted into weather buffer" << std::endl;
    }
}

// Method to dispatch messages from producers' queues
void Dispatcher::dispatch() {
    size_t numProducers = producerBuffers.size();
    size_t currentIndex = 0;

    while (!allProducersDone) {
        // Check the current producer's buffer
        BoundedBuffer* currentBuffer = producerBuffers[currentIndex];

        // Try to remove a message from the current producer's buffer
        char* rawMessage = currentBuffer->remove(); // Non-blocking removal
        if (rawMessage != nullptr) {
            std::string message = std::string(rawMessage);
            free(rawMessage); // Free the dynamically allocated string

            // Check for "DONE" message
            if (message == "DONE") {
                producerDone[currentIndex] = true; // Mark the producer as done
            } else {
                // Sort the message into the appropriate buffer
                sortMessage(message);
            }
        }

        // Check if all producers are done
        allProducersDone = true;
        for (bool done : producerDone) {
            if (!done) {
                allProducersDone = false;
                break;
            }
        }

        // Move to the next producer in the round-robin cycle
        currentIndex = (currentIndex + 1) % numProducers;
    }

    // When all producers are done, send "DONE" to all dispatcher buffers
    sportsBuffer.insert(const_cast<char*>("DONE"));
    newsBuffer.insert(const_cast<char*>("DONE"));
    weatherBuffer.insert(const_cast<char*>("DONE"));
}

// Check if all producers are done
bool Dispatcher::areAllProducersDone() {
    return allProducersDone;
}

BoundedBuffer& Dispatcher::getSportsBuffer() {
    return sportsBuffer;
}

BoundedBuffer& Dispatcher::getNewsBuffer() {
    return newsBuffer;
}

BoundedBuffer& Dispatcher::getWeatherBuffer() {
    return weatherBuffer;
}