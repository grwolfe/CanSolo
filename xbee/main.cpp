#include "Xbee.h"

Serial pc(USBTX, USBRX);

void processCom(const char* com) {
    pc.puts(com);
}

int main() {
    Xbee* xbee = new Xbee(p13, p14);
    char buffer[128];
    char message[128];

    while (1) {
        if(pc.readable()) {  //sending
            pc.gets(buffer, 4);
            xbee->xbee.puts(buffer);
        }
        if(xbee->xbee.readable()) {  //recieving
            printf("Recieved from Ground Station: ");
            xbee->xbee.gets(message, 2);

            xbee->processCommand(message);

        }
    }
}
