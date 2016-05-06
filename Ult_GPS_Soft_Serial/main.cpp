#include "mbed.h"
#include "SoftSerial.h"
#include "MBed_Adafruit_GPS.h"

SoftSerial* pc;
SoftSerial* gps;

int main() {
    pc = new SoftSerial(USBTX, USBRX);
    pc->baud(9600);
    
    gps = new SoftSerial(p11, p12);
    Adafruit_GPS myGPS(gps);
    char c;
    Timer refresh_Timer;
    const int refresh_Time = 2000;
    
    myGPS.begin(9600);
    pc->printf("here");
    
    myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    myGPS.sendCommand(PGCMD_ANTENNA);
    
    pc->printf("Connection established at 115200 baud...\n");
    
    refresh_Timer.start();
    
    while(true){
        c = myGPS.read();   //queries the GPS
        
        if (c) { pc->printf("%c", c); } //this line will echo the GPS data if not paused
        
        //check if we recieved a new message from GPS, if so, attempt to parse it,
        if ( myGPS.newNMEAreceived() ) {
            if ( !myGPS.parse(myGPS.lastNMEA()) ) {
                continue;   
            }    
        }
        
        //check if enough time has passed to warrant printing GPS info to screen
        //note if refresh_Time is too low or pc.baud is too low, GPS data may be lost during printing
        if (refresh_Timer.read_ms() >= refresh_Time) {
            refresh_Timer.reset();
            pc->printf("Time: %d:%d:%d.%u\n", myGPS.hour, myGPS.minute, myGPS.seconds, myGPS.milliseconds);   
            pc->printf("Date: %d/%d/20%d\n", myGPS.day, myGPS.month, myGPS.year);
            pc->printf("Fix: %d\n", (int) myGPS.fix);
            pc->printf("Quality: %d\n", (int) myGPS.fixquality);
            if (myGPS.fix) {
                pc->printf("Location: %5.2f%c, %5.2f%c\n", myGPS.latitude, myGPS.lat, myGPS.longitude, myGPS.lon);
                pc->printf("Speed: %5.2f knots\n", myGPS.speed);
                pc->printf("Angle: %5.2f\n", myGPS.angle);
                pc->printf("Altitude: %5.2f\n", myGPS.altitude);
                pc->printf("Satellites: %d\n", myGPS.satellites);
            }
        }
    }
}
