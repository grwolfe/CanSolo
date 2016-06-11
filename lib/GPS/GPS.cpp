#include "GPS.h"
GPS::GPS(PinName tx, PinName rx) : _UltimateGps(tx, rx)
{
    _UltimateGps.baud(9600);
}

int GPS::parseData()
{
    while(1) {
        getData();
        if(sscanf(NEMA, "GPGGA, %*f, %*f, %*c, %*f, %*c, %d, %d, %*f, %f", &fix, &satellites, &altitude) >=1);
        if(sscanf(NEMA, "GPRMC, %2d%2d%f, %c, %f, %c, %f, %c, %f, %f, %2d%2d%2d", &hours, &minutes, &seconds, &validity, &latitude, &ns, &longitude, &ew, &speed, &heading, &day, &month, &year) >=1) {
            if(fix == 0) {
                return 0;
            }
//            year += 2000;
//            if(ns =='S') {
//                latitude   *= -1.0;
//            }
//            if(ew =='W') {
//                longitude  *= -1.0;
//            }
//            float degrees = trunc(latitude / 100.0f);
//            float minutes = latitude - (degrees * 100.0f);
//            latitude = degrees + minutes / 60.0f;
//            degrees = trunc(longitude / 100.0f);
//            minutes = longitude - (degrees * 100.0f);
//            longitude = degrees + minutes / 60.0f;
//            if(fix == 1) {
//                fixtype = "Positive";
//            }
//            if(fix == 2) {
//                fixtype = "Differential";
            mps = speed*0.514;
            return 1;
        }
    }
}


float GPS::trunc(float v)
{
    if(v < 0.0) {
        v*= -1.0;
        v = floor(v);
        v*=-1.0;
    } else {
        v = floor(v);
    }
    return v;
}

void GPS::getData()
{
    while(_UltimateGps.getc() != '$');
    for(int i=0; i<256; i++) {
        NEMA[i] = _UltimateGps.getc();
        if(NEMA[i] == '\r') {
            NEMA[i] = 0;
            return;
        }
    }
    error("overflowed message limit");
}

void GPS::Init()
{
    wait(1);
    _UltimateGps.printf("$PMTK220,200*2C\r\n");
    wait(0.2);
    _UltimateGps.printf("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
    wait(1);
}
