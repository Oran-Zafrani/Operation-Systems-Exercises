#include "Dispatcher.h"


Dispatcher::Dispatcher(const std::vector<BoundedBuffer*>& buffers, int dispatcherBufferSize)
    : producerBuffers(buffers),
      sportsBuffer(dispatcherBufferSize),   
      newsBuffer(dispatcherBufferSize),     
      weatherBuffer(dispatcherBufferSize),  
      producerDone(buffers.size(), false), 
      allProducersDone(false) {}

// Helper function to sort messages into the appropriate dispatcher buffer
void Dispatcher::sortMessage(const std::string& message) {
    if (message.find("SPORTS") != std::string::npos) {
        sportsBuffer.insert(const_cast<char*>(message.c_str())); 
    } else if (message.find("NEWS") != std::string::npos) {
        newsBuffer.insert(const_cast<char*>(message.c_str())); 
    } else if (message.find("WEATHER") != std::string::npos) {
        weatherBuffer.insert(const_cast<char*>(message.c_str())); 
    }
}

void Dispatcher::dispatch() {
    size_t numProducers = producerBuffers.size();
    size_t currentIndex = 0;

    while (!allProducersDone) {

        BoundedBuffer* currentBuffer = producerBuffers[currentIndex];

        // Skip buffers that are already marked as "done" or are empty
        if (producerDone[currentIndex] || currentBuffer->isEmpty()) {
            currentIndex = (currentIndex + 1) % numProducers;
            continue;
        }

        // Try to remove a message from the current producer's buffer
        char* rawMessage = currentBuffer->remove(); 
        if (rawMessage != nullptr) {
            std::string message = std::string(rawMessage);
            free(rawMessage); // Free the dynamically allocated string

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