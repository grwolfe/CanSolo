#include "mbed.h"
#include "Camera_LS_Y201.h"
#include "SDFileSystem.h"

#define DEBMSG      printf
#define NEWLINE()   printf("\r\n")

#define USE_SDCARD 0

#if USE_SDCARD
#define FILENAME    "/sd/IMG_%04d.jpg"
SDFileSystem fs(p5, p6, p7, p8, "sd");
#else
#define FILENAME    "/local/IMG_%04d.jpg"
LocalFileSystem fs("local");
#endif
Camera_LS_Y201 cam1(p13, p14);

typedef struct work {
    FILE *fp;
} work_t;

work_t work;

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

/**
 * Entry point.
 */
int main(void) {
    DEBMSG("Camera module");
    NEWLINE();
    DEBMSG("Resetting...");
    NEWLINE();
    wait(1);

    if (cam1.reset() == 0) {
        DEBMSG("Reset OK.");
        NEWLINE();
    } else {
        DEBMSG("Reset fail.");
        NEWLINE();
        error("Reset fail.");
    }
    wait(1);

    int cnt = 0;
    while (1) {
        char fname[64];
        snprintf(fname, sizeof(fname) - 1, FILENAME, cnt);
        int r = capture(&cam1, fname);
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
