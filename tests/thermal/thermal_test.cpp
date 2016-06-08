#include "mbed.h"
#include "TMP102.h"

Serial pc(USBTX, USBRX);
Timer timeout;
PwmOut buzzer(p25);
TMP102 tmp1(p28, p27, 0x48 << 1);
TMP102 tmp2(p28, p27, 0x49 << 1);
int period = 250;

float avg_temp()
{
    // determine avg_temp
    float s1 = tmp1.read();
    float s2 = tmp2.read();
    float avg = (s1 + s2) / 2;
    printf("Sensor 1: %f C\r\nSensor 2: %f C\r\n", s1, s2);
    return avg;
}

int main()
{
    // initalize sensors
    tmp1.init();
    tmp2.init();
    timer.start();

    // begin program
    while (true) {
        float temp = avg_temp();
        if (temp <= 55 || temp >= 60 && timer.read() > 10)   // alert if reach temperature limits
        {
            buzzer.period_us(period);
            buzzer.pulsewidth_us(period/2);
            while (!pc.readable());     // after heat source toggle
            pc.getc();
            timer.reset();
            buzzer.write(0);
        } else {
            buzzer.write(0);
        }
        printf("Average: %f C\r\n", temp);
        wait(2.5);
    }
}
