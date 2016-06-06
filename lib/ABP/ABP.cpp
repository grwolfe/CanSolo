#include "ABP.h"

ABP::ABP(PinName sda, PinName scl, int address) : i2c_(sda, scl), addr_(address)
{
    // Creates i2c bus on given SDA & SCL pins
    // Assigns internal address param as given
}

ABP::~ABP()
{
    // destroys instance
}

int ABP::init(int freq)
{
    // implement an init function
    i2c_.frequency(freq);
    return 0;
}

float ABP::velocity()
{
    // dynamic pressure equation
    return sqrt(2 * (pressure() / DENSITY));
}

float ABP::pressure()
{
    int output = read();
    pressure_ = (float)(((output - OUT_MIN) * (P_MAX - P_MIN)) / (OUT_MAX - OUT_MIN)) + P_MIN;
    return pressure_;
}

int ABP::read()
{
    char data[2];
    i2c_.read(addr_, data, 2);  // Read data from sensor, 2 bytes indicate press
    if ((int8_t)data[0] >> 6 != 0) return -1;
    int16_t pres = ((int8_t) data[0] << 8) | ((uint8_t) data[1]);   // shift the first byte 8 bits to the left, combine with the 8 bits of the second byte
    return pres;        // return 16 bit int, bits 13:0 represent output
}
