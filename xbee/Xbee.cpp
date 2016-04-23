#include "Xbee.h"

Xbee::Xbee(PinName tx, PinName rx) : xbee(tx, rx) {

}

void Xbee::processCommand(char* com) {
     printf("Processing...");
     printf(com);
     printf("\r\n");

     if (com[0] == 'p') {
         printf("Picture command recieved.  Taking picture...");
         printf("\r\n");
     } else if (com[0] == 'r') {
         printf("Reset command recieved.  Resetting processor...");
         printf("\r\n");
     } else {
         printf("Unrecognized command");
         printf("\r\n");
     }
}
