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
    xbee.printf("Starting conversion\n");
    char* buffer = new char[4];
    int num = *((int*)&f);
    for (int i = 0; i < 4; i++) {
        buffer[3 - i] = (num >> 8 * i) & 0xFF;
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
    xbee.printf("Program Starting\n");
    float x = 13.4;
    char* bytes = floatToBytes(x);
    xbee.printf("%f\n", x);
    for (int i = 0; i < 4; i++) {
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
