#include "mbed.h"
#include "BMP180.h"
#include "TMP102.h"
#include "ABP.h"
#include "SoftSerial.h"
#include "MBed_Adafruit_GPS.h"
#include "Xbee.h"

#define size 28
#define COMMA 0x2C  // HEX code for comma separator
#define RETURN 0x0D // HEX code for carriage return

uint8_t     packet8[size];  // packet with 8 bit index [char]
uint16_t    packet16[size]; // packet with 16 bit index [int]

void init()
{
    packet16[0] = 0x116b;    // 0x116b = 4459 [TEAMID]
    packet16[1] = COMMA;
    packet8[0] = 0x11;
    packet8[1] = 0x6b;
    packet8[2] = COMMA;
}

int formPacket(uint16_t* i, uint8_t* c)
{

}

int main()
{

}
