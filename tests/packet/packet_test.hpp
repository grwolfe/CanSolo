#include "mbed.h"
#include "BMP180.h"
#include "TMP102.h"
#include "ABP.h"
#include "SoftSerial.h"
#include "MBed_Adafruit_GPS.h"

#define size 48     // size of the transmission packet
#define COMMA 0x2C  // HEX code for comma separator
#define RETURN 0x0D // HEX code for carriage return

uint8_t packet[size];   // packet with 8 bit index [char]
uint16_t packet_count;  // number of packets transmitted
Tiker timer;            // timer interrupt @ 1Hz
Timer burn;             // timer for burn wire

// ANALOG REFERENCES //
AnalogIn vref(p19);     // Voltage divider
AnalogIn lref(p20);     // Light sensor

// DIGITAL OUTPUTS //
DigitalOut mosfet(p30, 0);  // mosfet control signal inital state = LOW
PwmOut buzz(p21);

// GPS SETUP //
Adafruit_GPS gps(new SoftSerial(p11, p12));

// CAMERA SETUP //
Camera_LS_Y201 cam(p9, p10);

// XBEE SETUP //
Serial xbee(p13, p14);

// FUNCTION DEFINTIONS //
void init();
void transmit();
void processCommand();
void release();
void recovery();
float voltage();
float lref();
char* floatToBytes(float f);
char* intToBytes(int x);
