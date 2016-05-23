#ifndef ABP.H
#define ABP.H

#include "mbed.h"

#define P_MAX   30      // max pressure is 30 PSI
#define P_MIN   0       // min pressure is 0 PSI
#define OUT_MAX 16384   // output max
#define OUT_MIN 0       // output min

class ABP
{
    I2C i2c_;
    int addr_;
protected:
    int read();
public:
    // Constructor for ABP sensor object
    ABP(PinName sda, PinName scl, int address);
    ~ABP();
    float getPressure();
};

#endif
