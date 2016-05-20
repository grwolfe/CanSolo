#include "mbed.h"
#include "string"
#include "SoftSerial.h"
#include "MBed_Adafruit_GPS.h"
#include "Camera_LS_Y201.h"
//#include "SDHCFileSystem.h"

#define DEBMSG printf
#define NEWLINE() printf("\r\n")
#define FILENAME    "/local/img_%04d.jpg"


// I2C //
I2C i2c(p28, p27);  // Setup I2C bus

// ADC //
AnalogIn vref(p19); // Voltage divider reference
AnalogIn lref(p20); // Light sensor reference

// Digital Pins //
DigitalOut mosfet(p30, 0);  // initial state is low
PwmOut buzz(p21);


// File Systems //
// SDFileSystem fs(p5, p6, p7, p8, "sd");
LocalFileSystem fs("local");

// GPS //
SoftSerial* gps;

// Serial //
Serial xbee(p13, p14);
Serial pc(USBTX, USBRX);    // for communication with the xbee
Camera_LS_Y201 cam(p9, p10);

typedef struct work {
    FILE *fp;
} work_t;

work_t work;

// Functions //
void scan();        // scan i2c devices
void voltage();     // measure the analog voltage reference
void light();       // measure the analog light reference
void f_gps();       // queries the gps for a short time
void release();     // drive the release system
void recovery();    // drive the rcovery system
void radio();       // perform radio session
void image();       // take picture
// void memory();      // save data to memory
