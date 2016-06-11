#include "mbed.h"
#include "SoftSerial.h"
#include "GPS.h"

GPS gps(p11,p12);
float
    lat,            // gps latitude
    lon,            // gps longitude
    alt,            // gps altitude
    sat,            // gps sat number
    spd;            // gps speed
char
    latDirection,   // gps north or south
    lonDirection;   // gps east or west

void gps_query()
{
    if (gps.parseData()) {
        lat = gps.latitude;
        lon = gps.longitude;
        spd = gps.speed;
        sat = gps.satellites;
        alt = gps.altitude;
        latDirection = gps.ns;
        lonDirection = gps.ew;
    }
}

int main()
{
    gps.Init();
    while (1) {
        gps_query();
        printf("%f%c,%f%c,%f,%f,%f\r\n", lat, latDirection, lon, lonDirection, alt, sat, spd);
        wait(1);
    }
}
