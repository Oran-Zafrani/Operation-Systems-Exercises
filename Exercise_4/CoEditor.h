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
    CoEditor(BoundedBuffer& input, BoundedBuffer& shared, const std::string& editorType);

    void edit();
};

#endif 