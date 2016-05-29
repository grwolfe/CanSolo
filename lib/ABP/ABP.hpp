#ifndef ABP_H
#define ABP_H

#include "mbed.h"

#define DENSITY 1.225

#define P_MAX   30      // max pressure is 30 PSI
#define P_MIN   0       // min pressure is 0 PSI
#define OUT_MAX 14745   // output max
#define OUT_MIN 1638    // output min

class ABP
{
private:
    I2C i2c_;           // communication protocol
    int addr_;          // slave address
    float pressure_;    // value of last pressure reading

protected:
    int read();
    float pressure();

public:
    // Constructor for ABP sensor object
    ABP(PinName sda, PinName scl, int address);
    ~ABP();
    int init(int freq = 400000);
    float velocity();
};

#endif
