#include "mbed.h"
#include "string"
// #include "SDHCFileSystem.h"

// I2C //
I2C i2c(p28, p27);  // Setup I2C bus

// ADC //
AnalogIn vref(p19); // Voltage divider reference
AnalogIn lref(p20); // Light sensor reference

// SD //
SDFileSystem sd(p5, p6, p7, p8, "sd");


// Functions //
void scan();
void voltage();
void light();
// void memory();
