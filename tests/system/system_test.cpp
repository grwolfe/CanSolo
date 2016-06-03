#include "main.h"

int main()
{
    init();
}

void init()
{
    // set up TEAMID in packet
    packet[0] = 0x11;
    packet[1] = 0x6B;
    packet[2] = COMMA;

    // Initial packet count is 0
    packet[3] = 0x00;
    packet[4] = 0x00;
    packet[5] = COMMA;

    // Last byte of packet is carriage return
    packet[47] = RETURN;
    packet_count = 0;               // initalize packet counter to 0

    if (!bmp.Initalize()) xbee.printf("Error in BMP init\n");
    if (pitot.init()) xbee.printf("Error in ABP init\n");
    gps.begin(9600);

    xbee.attach(&processCommand);   // set up RX interrupt handler
    timer.attach(&transmit, 1);     // set up timed interrupt handler
}

void formPacket()
{
    // interact with sensors to
}

float voltage()
{
    int raw = vref.read_u16();
    float volts = 2 * ((raw * 3.3) / 65535);
    return volts;
}

float light()
{
    int raw = lref.read_u16(); // range 0 -> 65535
    float lux = pow(10, (raw * 3.3) / 65535);
    return lux;
}

void transmit()
{
    // transmit most recent packet @ 1Hz
    for (int i = 0; i < size; ++i) {
        xbee.putc(packet[i]);
    }
}

void processCommand()
{
    // process a command
    // char command = xbee.getc();
    switch (xbee.getc()) {
        case 'd':                               // deploy
            mosfet.write(1);                    // activate the burn wire
            burn_timeout(&deployCallback, 5);   // callback to shut off wire
            break;
        case 'p':   image_flag = true;      break;
        case 'r':   reset_flag = true;      break;
        case 'b':   recovery_flag = true;   break;
    }
}

void reset()
{
    // save everything to memory and reset processor
    // refresh from values and continue
}

void deploy()
{
    mosfet.write(1);
    burn_timeout.start();   // create a timeout to turn off the mosfet
}

void deployCallback() { mosfet.write(0); }

void recovery()
{
    shutdown();
    buzzer.write(1);
}

void saveState()
{
    // save everything needed to SD card
}

void shutdown()
{
    // shut everything off / save to memory
    saveState();
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
    // 2 bytes in an int, store MSB first
    char* buffer = new char[2];
    for (int i = 0; i < 2; ++i) {
        buffer[1 - i] = (x >> 8 * i) & 0xFF;
    }
    return buffer;
}
