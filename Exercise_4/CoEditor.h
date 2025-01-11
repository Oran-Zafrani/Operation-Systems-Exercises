#ifndef COEDITOR_H
#define COEDITOR_H

#include <string>
#include <thread>
#include <chrono>
#include "BoundedBuffer.h"

class CoEditor {
private:
    BoundedBuffer& inputBuffer;  // Dispatcher-provided buffer (weather/sports/news)
    BoundedBuffer& sharedBuffer; // Shared buffer for the Screen Manager
    std::string type;            // Type of the CoEditor (sports/news/weather)

public:
    // Constructor
    CoEditor(BoundedBuffer& input, BoundedBuffer& shared, const std::string& editorType);

    // Editing process; consumes messages from the input buffer and passes them to the shared buffer
    void edit();
};

#endif // COEDITOR_H