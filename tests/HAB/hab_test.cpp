#include "mbed.h"

Serial xbee(p13, p14);      // set up xbee TX = p13, RX = p14
DigitalOut mosfet(p21, 0);  // set up mosfet at p21, intial state LOW

int main()
{
    while (true) {
        if (xbee.readable())    // checks if anything has come to the xbee
        {
            xbee.printf("Command received...\r\n"); // xbee is readable, command received
            char c = xbee.getc();                   // take in received character
            if (c = 'r')                            // check that command is release
            {
                xbee.printf("Activating MOSFET...");
                mosfet.write(1);                    // drive mosfet pin HIGH
                xbee.printf("Done.\r\n");
                wait(5);                            // wait certain amount of time, function takes in seconds
            } else {
                xbee.printf("Unrecognized command.\r\n");   // if command isn't correct
            }
        } else {
            xbee.printf("Awaiting command...\r\n"); // xbee not readable, no command received
        }
    }
}
