#include "main.h"

int main()
{
    init();
    while (1)
    {
        // handle command raised flags
        if (image_flag) image();
        if (recovery_flag) recovery();

        // handle state transition flags
        /*
        *   STATE TRANSITION OVERVIEW:
        *   First milestone is seeing light from outside the rocket
        *   After light is seen, start checking our altitude to deploy @ 400m +/- 10m
        *   Once deployment routine has run a deployed flag will bypass these state flag checking
        *   If the deployment routine runs and SV does not release, manual override will be required
        */
        if (!deploy_flag)
        {
            if (light_flag) // if detected ejection from rocket
            {
                // get altitude
                float a;
                bmp.read(&a, NULL);
                if (a <= DEPLOY_ALT) deploy();
            } else if (light() > LIGHT_THRESHOLD) { // if not ejected check if ejected
                light_flag = true;
            }
        }
        gatherTLM();
        saveState();
    }
}

void init()
{
    set_time(0);        // set RTC to 0 seconds
    packet_count = 0;   // initalize packet counter to 0
    xbee.baud(9600);

    if (!bmp.Initialize())   // setup bmp sensor
        xbee.printf("Error in BMP init\n");

    if (pitot.init())       // setup pitot sensor
        xbee.printf("Error in ABP init\n");

    // initalize the GPS
    gps.begin(9600);
    gps.sendCommand(PMTK_SET_BAUD_9600);
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
    gps.sendCommand(PGCMD_NOANTENNA);

    // set up camera
    if (cam.reset() != 0)
        xbee.printf("Error setting up camera.\n");

    xbee.attach(&processCommand);   // set up RX interrupt handler
    timer.attach(&transmit, 1);     // set up timed interrupt handler
}

void gatherTLM()
{
    // interact with sensors to form packet
    bmp.read(&altitude, &pressure);
    speed = pitot.velocity();
    temperature = tmp.read();
    volt = voltage();
    gps_query();
    xbee.printf("Packet formed!\r");
}

void gps_query()
{
    // query the gps
    while (true) {
        char c = gps.read(); // query
        if (gps.newNMEAreceived() ) {
            if (!gps.parse(gps.lastNMEA()) ) {
                continue;
            }
        }
        if (gps.fix) {
            lat = gps.latitude;
            latDirection = gps.lat;
            lon = gps.longitude;
            lonDirection = gps.lon;
            alt = gps.altitude;
            sat = gps.satellites;
            spd = gps.speed;
        }
    }
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
    mission_time = time(NULL);  // update mission time
    ++packet_count;             // update packet counter
    printf("%d,%u,%u,%f,%f,%f,%f,%f,%f%c,%f%c,%f,%f,%f,%u,%u,%u,%u,%u,%u\r",
        TEAMID,
        mission_time,
        packet_count,
        altitude,
        pressure,
        speed,
        temperature,
        volt,
        lat,
        latDirection,
        lon,
        lonDirection,
        alt,
        sat,
        spd,
        com_time,
        com_count,
        light_flag,
        deploy_flag,
        image_flag,
        recovery_flag
    );
}

void saveState()
{
    // save everything needed to SD card

}

void processCommand()
{
    // process a command
    switch (xbee.getc()) {
        case 'd':                                   // deploy
            deploy_flag = true;
            mosfet.write(1);                        // activate the burn wire
            burn_timeout.attach(&deployCallback, 5);// callback to shut off wire
            break;
        case 'p':
            com_time = time(NULL);  // update time of picture command
            ++com_count;            // increment command counter
            image_flag = true;      // set flag to handle
            break;
        case 'b':   recovery_flag = true;   break;
    }
}

void deploy()
{
    deploy_flag = true;
    mosfet.write(1);    // activate the mosfet
    burn_timeout.attach(&deployCallback, 5);   // create a timeout to turn off the mosfet; @param seconds
}

void deployCallback()
{ mosfet.write(0); }

void recovery()
{
    gps_en.write(0);    // disable the gps
    timer.detach();     // end 1Hz transmission

    // activate the buzzer
    buzzer.period_us(370);
    buzzer.pulsewidth_us(185);
    while (1);          // hang until manual shutoff
}


void image()
{
    char fname[64];
    snprintf(fname, sizeof(fname) - 1, FILENAME, mission_time);
    if (capture(&cam, fname) != 0)
        xbee.printf("Caputer Failure.");
}

int capture(Camera_LS_Y201 *cam, char *filename) {
    /*
     * Take a picture.
     */
    if (cam->takePicture() != 0) {
        return -1;  // fail
    }

    /*
     * Open file.
     */
    work.fp = fopen(filename, "wb");
    if (work.fp == NULL) {
        return -2;  // fail
    }

    /*
     * Read the content.
     */
    if (cam->readJpegFileContent(callback_func) != 0) {
        fclose(work.fp);
        return -3;  // fail
    }
    fclose(work.fp);

    /*
     * Stop taking pictures.
     */
    cam->stopTakingPictures();

    return 0;
}

void callback_func(int done, int total, uint8_t *buf, size_t siz) {
    fwrite(buf, siz, 1, work.fp);   // here is where the jpeg is actually being written to the SD Card

    // static int n = 0;
    // int tmp = done * 100 / total;
    // if (n != tmp) {
    //     n = tmp;
    //     DEBMSG("Writing...: %3d%%", n);
    //     NEWLINE();
    // }
}
