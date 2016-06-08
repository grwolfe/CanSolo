#ifndef ABP_H
#define ABP_H

#include "mbed.h"

#define DENSITY 1.225   // normal air density
#define P_MAX   30      // max pressure is 30 PSI
#define P_MIN   0       // min pressure is 0 PSI
#define OUT_MAX 14745   // output max
#define OUT_MIN 1638    // output min

class ABP
{
private:
    I2C i2c_;           // communication protocol
    int addr_;          // slave address
    double pressure_;    // value of last pressure reading

protected:
    double PSItoPA(float psi) const;

public:
    // Constructor for ABP sensor object
    ABP(PinName sda, PinName scl, int address);
    ~ABP();
    int read();           // read raw value from sensor
    double pressure();          // determine pressure from sensor data
    int init(int freq = 400000);// Initialize the sensor and i2c bus
    double velocity();          // determine velocity based off sensor data
};

#endif
