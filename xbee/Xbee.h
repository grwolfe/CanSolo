#include "mbed.h"

class Xbee {
public:
    Serial xbee;

    Xbee(PinName tx, PinName rx);
    void processCommand(char* com);
};
