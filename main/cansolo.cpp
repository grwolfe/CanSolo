#include "cansolo.h"

int main()
{
    init();
    while (1)
    {
        // handle command raised flags
        if (image_flag) image();
        if (recovery_flag) recovery();
        if (reset_flag) reset();

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
            if (light_flag) // if detected ejection from rocket, but not deployed
            {
                // get altitude
                float t, p, a;
                bmp.ReadData(&t, &p, &a);
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
    set_time(0);    // begin mission time

    // clear mission information
    TEAMID = 4459;
    com_time = 0;
    packet_count = 0;
    com_count = 0;

    // clear control flags
    image_flag = false;
    recovery_flag = false;
    reset_flag = false;
    light_flag = false;
    deploy_flag = false;

    xbee.baud(9600);    // start XBee communication
    xbee.printf("Here...\r");

    // setup GPS
    gps.Init();

    // setup filesystem for recording data
    fs.mount();


    // setup bmp sensor
    if (!bmp.Initialize()) xbee.printf("Error in BMP init\r");
        else xbee.printf("BMP init success\r");

    // setup TMP sensor
    if (tmp.init()) xbee.printf("Error in TMP init\r");
        else xbee.printf("TMP init success\r");

    // setup pitot sensor
    if (pitot.init()) xbee.printf("Error in ABP init\r");
        else xbee.printf("ABP init success\r");


    // set up camera
    if (cam.reset() != 0) xbee.printf("Error setting up camera.\r");
        else xbee.printf("Camera reset success\r");

    // attach interrupts
    xbee.attach(&processCommand);   // set up RX interrupt handler
    timer.attach(&transmit, 1);     // set up timed interrupt handler

    xbee.printf("Initialized\r");
}

void gatherTLM()
{
    // interact with sensors to form packet
    bmp.ReadData(&tBMP, &pressure, &altitude);
    speed = pitot.velocity();
    temperature = tmp.read();
    volt = voltage();
    gps_query();
}

void gps_query()
{
    if (gps.parseData()) {
        lat = gps.latitude;
        lon = gps.longitude;
        spd = gps.mps;
        sat = gps.satellites;
        alt = gps.altitude;
        latDirection = gps.ns;
        lonDirection = gps.ew;
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
    xbee.printf("%u,%u,%u,%f,%f,%f,%f,%f,%f%c,%f%c,%f,%f,%f,%u,%u,%u,%u,%u,%u,%u,%f\r",
        TEAMID,
        time(NULL),
        ++packet_count,
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
        recovery_flag,
        gps.fix,
        light(),
        tBMP
    );
}

bool saveState()
{
    // save everything needed to SD card
    FILE *file = fopen("/sd/flightdata.csv", "a");
    if (file == NULL) return 0;
    fprintf(file, "%u,%u,%u,%f,%f,%f,%f,%f,%f%c,%f%c,%f,%f,%f,%u,%u,%u,%u,%u,%u,%u,%f\r",
        TEAMID,
        time(NULL),
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
        recovery_flag,
        gps.fix,
        light()
    );
    fclose(file);
    return 1;
}

void reset()
{
    reset_flag = false;
    // setup bmp sensor
    if (!bmp.Initialize()) xbee.printf("Error in BMP init\r");
        else xbee.printf("BMP init success\r");

    // setup TMP sensor
    if (tmp.init()) xbee.printf("Error in TMP init\r");
        else xbee.printf("TMP init success\r");

    // setup pitot sensor
    if (pitot.init()) xbee.printf("Error in ABP init\r");
        else xbee.printf("ABP init success\r");

    // Setup gps
    gps.Init();

    // set up camera
    if (cam.reset() != 0) xbee.printf("Error setting up camera.\r");
        else xbee.printf("Camera reset success\r");
}

void processCommand()
{
    // process a command
    char command = xbee.getc();
    switch (command) {
        case 'd':                                   // deploy
            deploy_flag = true;
            mosfet.write(1);                        // activate the burn wire
            burn_timeout.attach(&deployCallback,10);// callback to shut off wire
            break;
        case 'p':
            com_time = time(NULL);  // update time of picture command
            ++com_count;            // increment command counter
            image_flag = true;      // set flag to handle
            break;
        case 'b':   recovery_flag = true;   break;
        case 'r':   reset_flag = true;      break;
        default: printf("Unknown\r\n");
    }
}

void deploy()
{
    deploy_flag = true;
    mosfet.write(1);    // activate the mosfet
    burn_timeout.attach(&deployCallback,10);   // create a timeout to turn off the mosfet; @param seconds
}

void deployCallback()
{ mosfet.write(0); }

void recovery()
{
    timer.detach();     // end 1Hz transmission

    // activate the buzzer
    buzzer.period_us(BUZZ);
    buzzer.pulsewidth_us(BUZZ/2);
    while (1);          // hang until manual shutoff
}


void image()
{
    timer.detach();
    image_flag = false;
    char fname[64];
    snprintf(fname, sizeof(fname) - 1, FILENAME, time(NULL));
    if (capture(&cam, fname) != 0)
         xbee.printf("Capture Failure.");
    timer.attach(&transmit, 1);
}

int capture(Camera_LS_Y201 *cam, char *filename) {
    // Take picture
    if (cam->takePicture() != 0) return -1;  // fail

    // Open file
    work.fp = fopen(filename, "wb");
    if (work.fp == NULL) return -2;  // fail

    // Read the content
    if (cam->readJpegFileContent(callback_func) != 0) {
        fclose(work.fp);
        return -3;  // fail
    }
    fclose(work.fp);

    // Stop taking pictures
    cam->stopTakingPictures();
    return 0;
}

void callback_func(int done, int total, uint8_t *buf, size_t siz)
{ fwrite(buf, siz, 1, work.fp); }   // here is where the jpeg is actually being written to the SD Card
