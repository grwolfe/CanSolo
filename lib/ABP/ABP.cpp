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

double ABP::velocity()
{
    // dynamic pressure equation
    return sqrt(2 * (PSItoPA(pressure()) / DENSITY));
}

double ABP::pressure()
{
    float output = read();
    pressure_ = (float)(((output - OUT_MIN) * (P_MAX - P_MIN)) / (OUT_MAX - OUT_MIN)) + P_MIN;
    return pressure_;
}

int ABP::read()
{
    char data[2];
    i2c_.read(addr_, data, 2);  // Read data from sensor, 2 bytes indicate press
    data[0] &= ~(3 << 6); // clear bits 6 & 7; and with 0011 1111
    int16_t pres = ((int8_t) data[0] << 8) | ((uint8_t) data[1]);   // shift the first byte 8 bits to the left, combine with the 8 bits of the second byte
    printf("%x, %x\r\n%i\r\n", data[0], data[1], pres);
    return pres;        // return 16 bit int, bits 13:0 represent output
}

double ABP::PSItoPA(float psi) const
{ return psi * 6894.76; }
