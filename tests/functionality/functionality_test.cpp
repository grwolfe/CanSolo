#include "main.h"

/**
 * Callback function for readJpegFileContent.
 *
 * @param buf A pointer to a buffer.
 * @param siz A size of the buffer.
 */
void callback_func(int done, int total, uint8_t *buf, size_t siz) {
    fwrite(buf, siz, 1, work.fp);

    static int n = 0;
    int tmp = done * 100 / total;
    if (n != tmp) {
        n = tmp;
        DEBMSG("Writing...: %3d%%", n);
        NEWLINE();
    }
}

/**
 * Capture.
 *
 * @param cam A pointer to a camera object.
 * @param filename The file name.
 *
 * @return Return 0 if it succeed.
 */
int capture(Camera_LS_Y201 *cam, char *filename) {
    /*
     * Take a picture.
     */
    if (cam->takePicture() != 0) {
        return -1;
    }
    DEBMSG("Captured.");
    NEWLINE();

    /*
     * Open file.
     */
    work.fp = fopen(filename, "wb");
    if (work.fp == NULL) {
        return -2;
    }

    /*
     * Read the content.
     */
    DEBMSG("%s", filename);
    NEWLINE();
    if (cam->readJpegFileContent(callback_func) != 0) {
        fclose(work.fp);
        return -3;
    }
    fclose(work.fp);

    /*
     * Stop taking pictures.
     */
    cam->stopTakingPictures();

    return 0;
}

int main() {
    scan();
    wait(3);
    voltage();
    wait(3);
    light();
    wait(3);
    f_gps();
    wait(3);
    release();
    wait(3);
    radio();
    wait(3);
    image();
    wait(3);
    recovery();

    while(1);   // end program loop forever
}

void scan() {
    int error, address;
    int ndevices = 0;
    printf("Scanning...\r\n");
    for (address = 1; address < 127; address++)
    {
        i2c.start();
        error = i2c.write(address << 1);
        i2c.stop();
        if (error == 1)
        {
            printf("I2C device found at address 0x%X\r\n", address);
            ndevices++;
        }
    }
    if (ndevices == 0) printf("No I2C device found\r\n");
    else printf("\r\nDone\r\n\n");
}

void voltage() {
    int x = vref.read_u16();    // Normalized ADC value
    printf("Raw voltage reference = %i\r\n", x);
    double measured = ((x * 3.3)/65535);    // convert ADC value to the Voltage
    printf("Measured analog voltage = %f\r\n", measured);
    double actual = measured * 2;  // convert the measured voltage to the source voltage
    printf("Acutal voltage = %f\r\n\n", actual);
}

void light() {
    int raw = lref.read_u16();
    printf("Raw light sensor value = %i\r\n\n", raw);
}

// void memory() {
//     FILE* fp = fopen("/sd/fdata.csv", "w");
//     fprintf(fp, "1234, 5678, 91011, 121314, 151617, 181920\n\r");
//     fclose(fp);
//     printf("Saved data to SD card\r\n");
// }

void f_gps() {
    // gps = new SoftSerial(p11, p12);
    Adafruit_GPS myGPS(new SoftSerial(p11, p12));
    char c;
    Timer refresh_Timer;
    Timer timeout;
    const int refresh_Time = 2000;

    myGPS.begin(9600);
    printf("GPS Here...");
    myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    myGPS.sendCommand(PGCMD_ANTENNA);
    printf("Connection established at 9600 baud.\r\n");

    refresh_Timer.start();
    timeout.start();
    while(timeout.read() <= 3) {
        c = myGPS.read();   //queries the GPS

        if (c) { printf("%c", c); } //this line will echo the GPS data if not paused

        //check if we recieved a new message from GPS, if so, attempt to parse it,
        if ( myGPS.newNMEAreceived() ) {
            if ( !myGPS.parse(myGPS.lastNMEA()) ) {
                continue;
            }
        }

        //check if enough time has passed to warrant printing GPS info to screen
        //note if refresh_Time is too low or pc.baud is too low, GPS data may be lost during printing
        if (refresh_Timer.read_ms() >= refresh_Time) {
            refresh_Timer.reset();
            printf("Time: %d:%d:%d.%u\r\n", myGPS.hour, myGPS.minute, myGPS.seconds, myGPS.milliseconds);
            printf("Date: %d/%d/20%d\r\n", myGPS.day, myGPS.month, myGPS.year);
            printf("Fix: %d\r\n", (int) myGPS.fix);
            printf("Quality: %d\r\n", (int) myGPS.fixquality);
            if (myGPS.fix) {
                printf("Location: %5.2f%c, %5.2f%c\r\n", myGPS.latitude, myGPS.lat, myGPS.longitude, myGPS.lon);
                printf("Speed: %5.2f knots\r\n", myGPS.speed);
                printf("Angle: %5.2f\r\n", myGPS.angle);
                printf("Altitude: %5.2f\r\n", myGPS.altitude);
                printf("Satellites: %d\r\n", myGPS.satellites);
            }
        }
    }
    printf("\r\n");
}

void release() {
    printf("Driving MOSFET control pin high... ");
    mosfet.write(1);
    printf("Done\r\n");
    wait(2);
    printf("Driving MOSFET control pin low... ");
    mosfet.write(0);
    printf("Done\r\n\n");
}

void recovery() {
    printf("Activating buzzer...");
    buzz.write(0.5);
    printf("Done\r\n");
    wait(2);
    printf("Deactivating buzzer...");
    buzz.write(0);
    printf("Done\r\n\n");
}

void radio() {
    printf("Starting radio session. Enter q to quit...\r\n");
    while (true) {
        if (pc.readable())
        {
            char c = pc.getc();
            if (c == 'q') break;
            xbee.putc(c);
        } else if (xbee.readable()) {
            pc.putc(xbee.getc());
        }
    }
    printf("Ending radio session\r\n\n");
}

void image() {
    DEBMSG("Camera module");
    NEWLINE();
    DEBMSG("Resetting...");
    NEWLINE();
    wait(1);

    if (cam.reset() == 0) {
        DEBMSG("Reset OK.");
        NEWLINE();
    } else {
        DEBMSG("Reset fail.");
        NEWLINE();
        error("Reset fail.");
    }
    wait(1);

    int cnt = 0;
    while (cnt <= 3) {
        char fname[64];
        snprintf(fname, sizeof(fname) - 1, FILENAME, cnt);
        int r = capture(&cam, fname);
        if (r == 0) {
            DEBMSG("[%04d]:OK.", cnt);
            NEWLINE();
        } else {
            DEBMSG("[%04d]:NG. (code=%d)", cnt, r);
            NEWLINE();
            error("Failure.");
        }
        cnt++;
    }
}
