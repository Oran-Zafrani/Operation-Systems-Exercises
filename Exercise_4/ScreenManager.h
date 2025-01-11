#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <string>
#include "BoundedBuffer.h"

class ScreenManager {
private:
    BoundedBuffer& sharedBuffer; // The single shared buffer from the CoEditors
    int doneCount;               // Count of "DONE" messages received

public:
    // Constructor
    ScreenManager(BoundedBuffer& shared);

    // Process messages from the shared buffer
    void process();
};

#endif // SCREENMANAGER_H