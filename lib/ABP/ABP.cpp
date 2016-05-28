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

float ABP::getPressure()
{
    int output = read();
    float pressure = (((output - OUT_MIN) * (P_MAX - P_MIN)) / (OUT_MAX - OUT_MIN)) + P_MIN;
    return pressure;
}

int ABP::read()
{
    char data[2];
    i2c_.read(addr_, data, 2);  // Read data from sensor, 2 bytes indicate press
    // DEBUG("Byte 1: %c\r\nByte 2: %c\r\n", data[0], data[1]);
    int16_t pres = ((int8_t) data[0] << 8) | ((uint8_t) data[1]);   // shift the first byte 8 bits to the left, combine with the 8 bits of the second byte
    // DEBUG("Altered bits into int: %X\r\nInterger representation: %i", pres, pres);
    // clear bits 15 & 14
    pres = pres << 2;   // shift status bits out
    pres = pres >> 2;   // shift bits back to position
    // DEBUG("After shifting bits: %X\r\nInterger representation: %i", pres, pres);
    return pres;    // return 16 bit int, bits 13:0 represent output
}