#include "mbed.h"
#include "BMP180.h"
#include "TMP102.h"
#include "ABP.h"
#include "SoftSerial.h"
#include "MBed_Adafruit_GPS.h"
#include "Camera_LS_Y201.h"
#include "SDFileSystem.h"

#define SDA p28         // I2C data line
#define SCL p27         // I2C clock line
#define MOSI p5         // SPI MOSI line
#define MISO p6         // SPI MISO line
#define SCK  p7         // SPI clock line
#define SS   p8         // SPI slave select line
#define LIGHT_THRESHOLD 100
#define DEPLOY_ALT 400
#define FILENAME "/sd/img_%04u.jpg"

typedef struct work {
    FILE *fp;
} work_t;

work_t work;

// PACKET ENTRY DEFINITIONS //
uint16_t
    TEAMID,         // TEAMID for CanSolo
    mission_time,   // number of seconds since power on
    packet_count,   // number of packets transmitted
    com_count,      // number of image commands received
    com_time;       // time of last image command
float
    altitude,       // altimeter measurement
    pressure,       // pressure sensor reading
    speed,          // pitot tube reading
    temperature,    // temp sensor reading
    volt,           // battery voltage
    lat,            // gps latitude
    lon,            // gps longitude
    alt,            // gps altitude
    sat,            // gps sat number
    spd;            // gps speed
char
    latDirection,   // gps north or south
    lonDirection;   // gps east or west

// CONTROL FLAG DEFINTIONS //
bool
    image_flag,     // flag to handle picture command
    recovery_flag,  // flag to handle recovery command
    light_flag,     // flag to track released from rocket
    deploy_flag;    // flag to track deployment conditions

// MBED OBJECT DEFINTIONS //
Ticker timer;                               // timer interrupt @ 1Hz
Timeout burn_timeout;                       // timeout for burn wire
AnalogIn vref(p20);                         // voltage divider
AnalogIn lref(p19);                         // light sensor
DigitalOut mosfet(p21, 0);                  // mosfet control; inital LOW
DigitalOut gps_en(p16, 1);                  // GPS enable pin, bring LOW to disable
DigitalOut xbee_en(p15, 1);                 // XBee radio enable, bring LOW to disable
PwmOut buzzer(p25);                         // buzzer control signal
Serial xbee(p13, p14);                      // setup xbee as serial connection

// IMPORTED OBJECT DEFINTIONS //
Adafruit_GPS gps(new SoftSerial(p11, p12)); // create GPS object
SDFileSystem fs(MOSI, MISO, SCK, SS, "sd"); // setup MicroSD filesystem
Camera_LS_Y201 cam(p9, p10);                // create camera object
TMP102 tmp(SDA, SCL, 0x49 << 1);            // create TMP102 object
BMP180 bmp(SDA, SCL, 0x77 << 1);            // create BMP180 object
ABP  pitot(SDA, SCL, 0x28 << 1);            // create ABP2A3 object

// FUNCTION DEFINTIONS //
void init();                    // initalize function
void gatherTLM();               // gather telemetry from devices
void gps_query();               // function to handle queries to the GPS and parsing NMEA
float voltage();                // determine battery voltage from analog reference
float light();                  // determine light levels from analog reference
void transmit();                // 1Hz interrupt function to send packet
bool saveState();               // save all relevant data to memory
void reset();                   // software reset
void processCommand();          // RX interrupt function to process commands
void deploy();                  // activate the mosfet and give a callback to deactivate
void deployCallback();          // callback function to deactivate mosfet
void recovery();                // recovery mode
void image();                   // function to handle image commands and take picture
int capture(Camera_LS_Y201 *cam, char *filename);
void callback_func(int done, int total, uint8_t *buf, size_t siz);
