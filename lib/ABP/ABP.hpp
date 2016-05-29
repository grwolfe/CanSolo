#ifndef ABP_H
#define ABP_H

#include "mbed.h"

// #define DEBUG printf

#define P_MAX   30      // max pressure is 30 PSI
#define P_MIN   0       // min pressure is 0 PSI
#define OUT_MAX 14745   // output max
#define OUT_MIN 1638    // output min

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
    float pressure();
    float velocity();
    int init(int freq);
};

#endif
