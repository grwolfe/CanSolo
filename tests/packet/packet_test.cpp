#include "mbed.h"
Serial xbee(p13, p14);

// TEST OF RX INTERRUPT FUNCTIONALITY
// DigitalOut led(LED1);
// void processCommand()
// {
//     // process
//     xbee.printf("Command received... %c\r\n", xbee.getc());
//     led = !led;
// }
//
// int main()
// {
//     xbee.baud(9600);
//     xbee.attach(&processCommand);
//     int count = 0;
//     while (true)
//     {
//         xbee.printf("Normal loop count: %i\r\n", count);
//         count++;
//     }
// }

// TEST OF HOW NUMBERS ARE SENT WITH DIFFERENT COMMANDS
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

int main()
{
    // uint16_t buffer[5] = {0, 10, 200, 3000, 0xffff};
    // float buff[5] = {0, 13.4, 27.4, 198.689, 5.0};
    // xbee.printf("putc() command\n");
    // for (int i = 0; i < 5; i++) {
    //     xbee.putc(buffer[i]);
    //     xbee.putc(0x0a);
    // }
    // xbee.printf("\nprintf() command\n");
    // for (int i = 0; i < 5; i++) {
    //     xbee.printf("%i\n", buffer[i]);
    // }
    // xbee.printf("\nTesting with floats:\n");
    // for (int i = 0; i < 5; i++) {
    //     xbee.printf("%f\t%x\t%a\n", buff[i], buff[i], buff[i]);
    // }
    // xbee.printf("\n");
    char* bytes = new char[8];
    int y = 54;
    char* f = intToBytes(y);
    bytes[0] = f[0];
    bytes[1] = f[1];
    bytes[2] = 0x2C;
    float x = 13.4;
    f = floatToBytes(x);
    bytes[3] = f[0];
    bytes[4] = f[1];
    bytes[5] = f[2];
    bytes[6] = f[3];
    bytes[7] = 0x0D;
    for (int i = 0; i < 8; i++) {
        xbee.putc(bytes[i]);
    }
    while (1);
}

// TEST INTERACTION OF DIFFERENT INTERRUPTS
// Ticker timer;   // timed interrupt @ 1Hz
//
// void processCommand()
// {
//     xbee.printf("Command received...\r");
//     char command = xbee.getc();
//     switch (command) {
//         case 'd': xbee.printf("Deploy command\n");  break;
//         case 'p': xbee.printf("Picture command\n"); break;
//         case 'r': xbee.printf("Reset command\n");   break;
//         case 'q': xbee.printf("Quitting\n");        while(1);
//         default: xbee.printf("Unknown command\n");
//     }
// }
//
// void transmit()
// {
//     // transmit @ 1Hz
//     xbee.printf("transmitting packet\r");
// }
//
// int main()
// {
//     xbee.baud(9600);
//     xbee.attach(&processCommand);   // will call processCommand() on RX
//     timer.attach(&transmit, 1);     // will call transmit() on 1 second interval
//     int count = 0;
//     while (true)
//     {
//         xbee.printf("Normal loop count: %i\r", count);
//         count++;
//         wait(1);
//     }
// }
