#include "mbed.h"
#include "BMP180.h"
#include "TMP102.h"
#include "ABP.h"

// Definitions //
#define SDA p28
#define SCL p27
#define TMP_ADD 0x48 << 1
#define BMP_ADD 0x77 << 1
#define ABP_ADD 0x28 << 1

// Construct peripheral devices
TMP102 tmp(SDA, SCL, TMP_ADD);
BMP180 bmp(SDA, SCL, BMP_ADD);
ABP  pitot(SDA, SCL, ABP_ADD);
Serial xbee(p13, p14);

int main() {
    float bmp_pres, bmp_temp, tmp_temp, pitot_pres;
    while(1) {
        if (bmp.ReadData(bmp_temp, bmp_pres))
        {
            xbee.printf("BMP Pressure: %f\r\n", bmp_pres);
            xbee.printf("BMP Temperature: %f\r\n", bmp_temp);
        } else {
            xbee.printf("Error in reading data from BMP\r\n");
        }
        tmp_temp = tmp.read();
        xbee.printf("TMP Temperature: %f\r\n", tmp_temp);
        pitot_pres = pitot.getPressure();
        xbee.printf("Pitot Pressure: %f\r\n", pitot_pres);
    }
}
