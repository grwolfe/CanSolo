#include "mbed.h"
#include "TMP102.h"

DigitalOut led[4] = { LED1, LED2, LED3, LED4 };
TMP102 tmp1(p28, p27, 0x48 << 1);
TMP102 tmp2(p28, p27, 0x49 << 1);
const int period = 250;

float avg_temp()
{
    // determine avg_temp
    float s1 = tmp1.read();
    float s2 = tmp2.read();
    float avg = (s1 + s2) / 2;
    printf("Sensor 1:\t %f C\r\nSensor 2:\t %f C\r\n", s1, s2);
    return avg;
}

int main()
{
    // initalize sensors
    tmp1.init();
    tmp2.init();

    // begin program
    while (true) {
        float temp = avg_temp();
        printf("Average:\t %f C\r\n", temp);
        if (temp <= 55 || temp >= 60 && timer.read() > 10)   // alert if reach temperature limits
        {
            for (int i = 0; i < 4; i++) {
                led[i].write(1);
            }
        } else {
            for (int i = 0; i < 4; i++) {
                led[i].write(0);
            }
        }
        wait(3);
    }
}
