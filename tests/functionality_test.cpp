#include "main.h"

int main() {
    scan();
    wait(3);
    voltage();
    wait(3);
    light();
    wait(3);
    // memory();
    wait(3);

    while(1);   // loop forever
}

void scan() {
    // I2C scan
    int error, address;
    int nDevices = 0;
    printf("Scanning...\r\n");
    for (address = 1; address < 127; address++)
    {
        i2c.start();
        error = i2c.write(address << 1);
        i2c.stop();
        if (error == 1)
        {
            printf("I2C device found at address 0x%X\r\n", address);
            nDevices++;
        }
    }
    if (nDevices == 0) printf("No I2C device found\r\n");
    else printf("\r\nDone\r\n");
}

void voltage() {
    int x = vref.read_u16();    // Normalized ADC value
    printf("Raw voltage reference = %i\r\n", x);
    int measured = ((x * 3.3)/65535);    // convert ADC value to the Voltage
    printf("Measured analog voltage = %i\r\n", measured);
    int actual = measured * 2;  // convert the measured voltage to the source voltage
    printf("Acutal voltage = %i\r\n", actual);
}

void light() {
    int raw = lref.read_u16();
    printf("Raw light sensor value = %i\r\n", raw);
}

// void memory() {
//     FILE* fp = fopen("/sd/fdata.csv", "w");
//     fprintf(fp, "1234, 5678, 91011, 121314, 151617, 181920\n\r");
//     fclose(fp);
//     printf("Saved data to SD card\r\n");
// }
