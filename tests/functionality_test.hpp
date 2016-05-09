#include "mbed.h"
#include "string"
// #include "SDHCFileSystem.h"
#include "SoftSerial.h"
#include "MBed_Adafruit_GPS.h"

// I2C //
I2C i2c(p28, p27);  // Setup I2C bus

// ADC //
AnalogIn vref(p19); // Voltage divider reference
AnalogIn lref(p20); // Light sensor reference

// Digital Pins //
DigitalOut mosfet(p30, 0);  // initial state is low
PwmOut buzz(p21);


// SD //
// SDFileSystem sd(p5, p6, p7, p8, "sd");

// GPS //
SoftSerial* gps;

// Serial //
Serial pc(USBTX, USBRX);    // for communication with the xbee

// Functions //
void scan();
void voltage();
void light();
void f_gps();
void release();
void recovery();
// void memory();
