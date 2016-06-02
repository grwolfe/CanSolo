#include "mbed.h"
#include "SoftSerial.h"
#include "MBed_Adafruit_GPS.h"
#include "TMP102.h"
#include "BMP180.h"
#include "ABP.h"
#include "Camera_LS_Y201.h"

///////////////////////
// ANALOG REFERENCES //
///////////////////////
AnalogIn vref(p19); // Voltage divider
AnalogIn lref(p20); // Light sensor

/////////////////////
// DIGITAL OUTPUTS //
/////////////////////
DigitalOut mosfet(p30, 0);  // mosfet control signal inital state = LOW
PwmOut buzz(p21);

///////////////
// GPS SETUP //
///////////////
Adafruit_GPS gps(new SoftSerial(p11, p12));

//////////////////
// CAMERA SETUP //
//////////////////
Camera_LS_Y201 cam(p9, p10);

////////////////
// XBEE SETUP //
////////////////
Serial xbee(p13, p14);
