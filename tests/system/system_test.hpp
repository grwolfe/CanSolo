#include "mbed.h"
#include "BMP180.h"
#include "TMP102.h"
#include "ABP.h"
#include "SoftSerial.h"
#include "MBed_Adafruit_GPS.h"
#include "Camera_LS_Y201.h"
#include "SDHCFileSystem.h"

#define size 61         // size of the transmission packet
#define COMMA  0x2C     // HEX code for comma separator
#define RETURN 0x0D     // HEX code for carriage return
#define SDA p28         // I2C data line
#define SCL p27         // I2C clock line
#define MOSI p5         // SPI MOSI line
#define MISO p6         // SPI MISO line
#define SCK  p7         // SPI clock line
#define SS   p8         // SPI slave select line
#define FILENAME "/sd/img_%04d.jpg"

// OBJECT DEFINTIONS //
uint8_t packet[size];                       // packet with 8 bit index [char]
uint16_t packet_count;                      // number of packets transmitted
bool image_flag = false;                    // flag to handle picture command
bool reset_flag = false;                    // flag to handle reset command
bool recovery_flag = false;                 // flag to handle recovery command
Tiker timer;                                // timer interrupt @ 1Hz
Timeout burn_timeout;                       // timer for burn wire
AnalogIn vref(p20);                         // voltage divider
AnalogIn lref(p19);                         // light sensor
DigitalOut mosfet(p30, 0);                  // mosfet control; inital LOW
PwmOut buzzer(p21);                         // buzzer control signal
Adafruit_GPS gps(new SoftSerial(p11, p12)); // create GPS object
Camera_LS_Y201 cam(p9, p10);                // create camera object
Serial xbee(p13, p14);                      // setup xbee as serial connection
TMP102 tmp(SDA, SCL, 0x48 << 1);            // create TMP102 object
BMP180 bmp(SDA, SCL, 0x77 << 1;             // create BMP180 object
ABP  pitot(SDA, SCL, 0x28 << 1);            // create ABP object
SDFileSystem fs(MOSI, MISO, SCK, SS, "sd"); // setup MicroSD filesystem

// FUNCTION DEFINTIONS //
void init();                    // initalize function
void transmit();                // 1Hz interrupt function to send packet
void processCommand();          // RX interrupt function to process commands
void recovery();                // recovery mode
void reset();                   // reset processor
void deploy();                  // activate the mosfet and give a callback to deactivate
void deployCallback();          // callback function to deactivate mosfet
void saveState();               // save all relevant data to memory
void shutdown();                // save data & shutdown electronics (for aborts & recovery)
char* floatToBytes(float f);    // convert floating point to byte array (32 bits - char[4])
char* intToBytes(int x);        // convert int to byte array (16 bits - char[2])
float voltage();                // determine battery voltage from analog reference
float light();                   // determine light levels from analog reference
