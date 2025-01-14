#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <vector>
#include <iostream>
#include "BoundedBuffer.h"

class Dispatcher {
private:
    std::vector<BoundedBuffer*> producerBuffers; // Vector of producer buffers
    BoundedBuffer sportsBuffer;                 // SPORTS dispatcher buffer
    BoundedBuffer newsBuffer;                   // NEWS dispatcher buffer
    BoundedBuffer weatherBuffer;                // WEATHER dispatcher buffer
    std::vector<bool> producerDone;             // Tracks which producers are done
    bool allProducersDone;                      // Flag to indicate all producers are done

    // Helper function to sort messages into the appropriate dispatcher buffer
    void sortMessage(const std::string& message);

public:
    Dispatcher(const std::vector<BoundedBuffer*>& buffers, int dispatcherBufferSize);

    void dispatch();

    bool areAllProducersDone();

    BoundedBuffer& getSportsBuffer();
    BoundedBuffer& getNewsBuffer();
    BoundedBuffer& getWeatherBuffer();
};

#endif 