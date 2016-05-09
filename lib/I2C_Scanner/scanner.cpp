// Original program found here: http://playground.arduino.cc/Main/I2cScanner
// Modified by Domen Ornik, 4.5.2015

#include "mbed.h"

I2C i2c(p9, p10);

int main() {
    printf("\n\nI2C Scanner\r\n");

    while(1) {
        int error, address;
        int nDevices;

        printf("Scanning...\r\n");

         nDevices = 0;

          for(address = 1; address < 127; address++ )
          {
            i2c.start();
            error = i2c.write(address << 1); //We shift it left because mbed takes in 8 bit addreses
            i2c.stop();
            if (error == 1)
            {
              printf("I2C device found at address 0x%X", address); //Returns 7-bit addres
              nDevices++;
            }

          }
          if (nDevices == 0)
            printf("No I2C devices found\r\n");
          else
            printf("\r\ndone\r\n");

          wait(5);           // wait 5 seconds for next scan
    }
}
