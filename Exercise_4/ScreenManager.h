#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <string>
#include "BoundedBuffer.h"

class ScreenManager {
private:
    BoundedBuffer& sharedBuffer; // The single shared buffer
    int doneCount;               // Count of "DONE" messages received

public:
    ScreenManager(BoundedBuffer& shared);

    void process();
};

#endif 