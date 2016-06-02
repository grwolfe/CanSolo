#include "main.h"

int main()
{
    init();
}

void init()
{
    packet_count = 0;

    packet[0] = 0x11;
    packet[1] = 0x6b;
    packet[2] = COMMA;

    packet[3] = 0x00;
    packet[4] = 0x00;
    packet[5] = COMMA;

    packet[47] = RETURN;

    xbee.attach(&processCommand);
    timer.attach(&transmit, 1);
}

void transmit()
{
    // transmit packet @ 1Hz
    for (int i = 0; i < size; ++i) {
        xbee.putc(packet[i]);
    }
}

void processCommand()
{
    // process a command
    char command = xbee.getc();
    switch (command) {
        case 'd':
        case 'p':
        case 'r':
        case 'q':
        default: xbee.printf("Unknown\n");
    }
}

char* floatToBytes(float f)
{
    // convert a float into it's bytes
    // 4 bytes in a float, store MSB first
    char* buffer = new char[4];
    int num = *((int*)&f);
    for (int i = 0; i < 4; i++) {
        buffer[3 - i] = (num >> 8 * i) & 0xFF;
    }
    return buffer;
}

char* intToBytes(int x)
{
    // convert an int into it's bytes
    // 2 - 4 bytes in an int, store MSB 1st
    char* buffer = new char[2];
    for (int i = 0; i < 2; ++i)
    {
        buffer[1 - i] = (x >> 8 * i) & 0xFF;
    }
    return buffer;
}
