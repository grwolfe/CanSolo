#include "mbed.h"
#include "SoftSerial.h"
#include "MBed_Adafruit_GPS.h"

SoftSerial* gps;

int main() {
    gps = new SoftSerial(p11, p12);
    Adafruit_GPS myGPS(gps);
    char c;
    Timer refresh_Timer;
    const int refresh_Time = 2000;

    myGPS.begin(9600);
    printf("here\r\n");

    myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
    myGPS.sendCommand(PGCMD_ANTENNA);

    printf("Connection established at 9600 baud...\r\n");

    refresh_Timer.start();

    while(true){
        c = myGPS.read();   //queries the GPS

//        if (c) { printf("%c", c); } //this line will echo the GPS data if not paused

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
            printf("Time: %d:%d:%d.%u\r\n", myGPS.hour, myGPS.minute, myGPS.seconds, myGPS.milliseconds);
            printf("Date: %d/%d/20%d\r\n", myGPS.day, myGPS.month, myGPS.year);
            printf("Fix: %d\r\n", (int) myGPS.fix);
            printf("Quality: %d\r\n", (int) myGPS.fixquality);
            if (myGPS.fix) {
                printf("Location: %5.2f%c, %5.2f%c\r\n", myGPS.latitude, myGPS.lat, myGPS.longitude, myGPS.lon);
                printf("Speed: %5.2f knots\r\n", myGPS.speed);
                printf("Angle: %5.2f\r\n", myGPS.angle);
                printf("Altitude: %5.2f\r\n", myGPS.altitude);
                printf("Satellites: %d\r\n\n", myGPS.satellites);
            }
        }
    }
}
