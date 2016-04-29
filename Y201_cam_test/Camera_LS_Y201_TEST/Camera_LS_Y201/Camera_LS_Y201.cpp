/**
 * =============================================================================
 * LS-Y201 device driver class (Version 0.0.1)
 * Reference documents: LinkSprite JPEG Color Camera Serial UART Interface
 *                      January 2010
 * =============================================================================
 * Copyright (c) 2010 Shinichiro Nakamura (CuBeatSystems)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * =============================================================================
 */

#include "Camera_LS_Y201.h"

/**
 * Create.
 *
 * @param tx Transmitter.
 * @param rx Receiver.
 */
Camera_LS_Y201::Camera_LS_Y201(PinName tx, PinName rx) : serial(tx, rx) {
    serial.baud(38400);
}

/**
 * Dispose.
 */
Camera_LS_Y201::~Camera_LS_Y201() {
}

/**
 * Reset module.
 *
 * @return Error code.
 */
Camera_LS_Y201::ErrorCode Camera_LS_Y201::reset() {
    uint8_t send[4] = {
        0x56,
        0x00,
        0x26,
        0x00
    };
    uint8_t recv[4];

    waitIdle();
    if (!sendBytes(send, sizeof(send), 200 * 1000)) {
        return SendError;
    }
    if (!recvBytes(recv, sizeof(recv), 200 * 1000)) {
        return RecvError;
    }
    if ((recv[0] == 0x76)
            && (recv[1] == 0x00)
            && (recv[2] == 0x26)
            && (recv[3] == 0x00)) {
        ErrorCode r = waitInitEnd();
        if (r != NoError) {
            return r;
        }
        wait(4);
        return NoError;
    } else {
        return UnexpectedReply;
    }
}

/**
 * Set image size.
 *
 * @param is Image size.
 * @return Error code.
 */
Camera_LS_Y201::ErrorCode Camera_LS_Y201::setImageSize(ImageSize is) {
    uint8_t send[9] = {
        0x56,
        0x00,
        0x31,
        0x05,
        0x04,
        0x01,
        0x00,
        0x19,
        0x00    // 0x11:320x240, 0x00:640x480, 0x22:160x120
    };
    uint8_t recv[5];
    switch (is) {
        case ImageSize160x120:
            send[8] = 0x22;
            break;
        case ImageSize320x280:
            send[8] = 0x11;
            break;
        case ImageSize640x480:
            send[8] = 0x00;
            break;
        default:
            return InvalidArguments;
    }
    if (!sendBytes(send, sizeof(send), 200 * 1000)) {
        return SendError;
    }
    if (!recvBytes(recv, sizeof(recv), 200 * 1000)) {
        return RecvError;
    }
    if ((recv[0] == 0x76)
            && (recv[1] == 0x00)
            && (recv[2] == 0x31)
            && (recv[3] == 0x00)
            && (recv[4] == 0x00)) {
        wait(1);
        return reset();
    } else {
        return UnexpectedReply;
    }
}

/**
 * Take picture.
 *
 * @return Error code.
 */
Camera_LS_Y201::ErrorCode Camera_LS_Y201::takePicture() {
    uint8_t send[5] = {
        0x56,
        0x00,
        0x36,
        0x01,
        0x00
    };
    uint8_t recv[5];

    if (!sendBytes(send, sizeof(send), 200 * 1000)) {
        return SendError;
    }
    if (!recvBytes(recv, sizeof(recv), 200 * 1000)) {
        return RecvError;
    }

    if ((recv[0] == 0x76)
            && (recv[1] == 0x00)
            && (recv[2] == 0x36)
            && (recv[3] == 0x00)
            && (recv[4] == 0x00)) {
        /*
         * I think the camera need a time for operating.
         * But there is no any comments on the documents.
         */
        wait_ms(100);
        return NoError;
    } else {
        return UnexpectedReply;
    }
}

/**
 * Read jpeg file size.
 *
 * @param fileSize File size.
 * @return Error code.
 */
Camera_LS_Y201::ErrorCode Camera_LS_Y201::readJpegFileSize(int *fileSize) {
    uint8_t send[5] = {
        0x56,
        0x00,
        0x34,
        0x01,
        0x00
    };
    uint8_t recv[9];

    if (!sendBytes(send, sizeof(send), 200 * 1000)) {
        return SendError;
    }
    if (!recvBytes(recv, sizeof(recv), 200 * 1000)) {
        return RecvError;
    }

    if ((recv[0] == 0x76)
            && (recv[1] == 0x00)
            && (recv[2] == 0x34)
            && (recv[3] == 0x00)
            && (recv[4] == 0x04)
            && (recv[5] == 0x00)
            && (recv[6] == 0x00)) {
        *fileSize = ((recv[7] & 0x00ff) << 8)
                    | ((recv[8] & 0x00ff) << 0);
        return NoError;
    } else {
        return UnexpectedReply;
    }
}

/**
 * Read jpeg file content.
 *
 * @param func A pointer to a call back function.
 * @return Error code.
 */
Camera_LS_Y201::ErrorCode Camera_LS_Y201::readJpegFileContent(void (*func)(int done, int total, uint8_t *buf, size_t siz)) {
    uint8_t send[16] = {
        0x56,
        0x00,
        0x32,
        0x0C,
        0x00,
        0x0A,
        0x00,
        0x00,
        0x00, // MH
        0x00, // ML
        0x00,
        0x00,
        0x00, // KH
        0x00, // KL
        0x00, // XX
        0x00  // XX
    };
    uint8_t body[32];
    uint16_t m = 0; // Staring address.
    uint16_t k = sizeof(body); // Packet size.
    uint16_t x = 10;    // Interval time. XX XX * 0.01m[sec]
    bool end = false;

    /*
     * Get the data size.
     */
    int siz_done = 0;
    int siz_total = 0;
    ErrorCode r = readJpegFileSize(&siz_total);
    if (r != NoError) {
        return r;
    }

    do {
        send[8] = (m >> 8) & 0xff;
        send[9] = (m >> 0) & 0xff;
        send[12] = (k >> 8) & 0xff;
        send[13] = (k >> 0) & 0xff;
        send[14] = (x >> 8) & 0xff;
        send[15] = (x >> 0) & 0xff;
        /*
         * Send a command.
         */
        if (!sendBytes(send, sizeof(send), 200 * 1000)) {
            return SendError;
        }
        /*
         * Read the header of the response.
         */
        uint8_t header[5];
        if (!recvBytes(header, sizeof(header), 2 * 1000 * 1000)) {
            return RecvError;
        }
        /*
         * Check the response and fetch an image data.
         */
        if ((header[0] == 0x76)
                && (header[1] == 0x00)
                && (header[2] == 0x32)
                && (header[3] == 0x00)
                && (header[4] == 0x00)) {
            if (!recvBytes(body, sizeof(body), 2 * 1000 * 1000)) {
                return RecvError;
            }
            siz_done += sizeof(body);
            if (func != NULL) {
                if (siz_done > siz_total) {
                    siz_done = siz_total;
                }
                func(siz_done, siz_total, body, sizeof(body));
            }
            for (int i = 1; i < sizeof(body); i++) {
                if ((body[i - 1] == 0xFF) && (body[i - 0] == 0xD9)) {
                    end = true;
                }
            }
        } else {
            return UnexpectedReply;
        }
        /*
         * Read the footer of the response.
         */
        uint8_t footer[5];
        if (!recvBytes(footer, sizeof(footer), 2 * 1000 * 1000)) {
            return RecvError;
        }

        m += sizeof(body);
    } while (!end);
    return NoError;
}

/**
 * Stop taking pictures.
 *
 * @return Error code.
 */
Camera_LS_Y201::ErrorCode Camera_LS_Y201::stopTakingPictures() {
    uint8_t send[5] = {
        0x56,
        0x00,
        0x36,
        0x01,
        0x03
    };
    uint8_t recv[5];

    if (!sendBytes(send, sizeof(send), 200 * 1000)) {
        return SendError;
    }
    if (!recvBytes(recv, sizeof(recv), 200 * 1000)) {
        return RecvError;
    }

    if ((recv[0] == 0x76)
            && (recv[1] == 0x00)
            && (recv[2] == 0x36)
            && (recv[3] == 0x00)
            && (recv[4] == 0x00)) {
        /*
         * I think the camera need a time for operating.
         * But there is no any comments on the documents.
         */
        wait_ms(100);
        return NoError;
    } else {
        return UnexpectedReply;
    }
}

/**
 * Wait init end codes.
 *
 * @return True if the data sended.
 */
Camera_LS_Y201::ErrorCode Camera_LS_Y201::waitInitEnd() {
    static const char *PWR_ON_MSG = "Init end\x0d\x0a";
    for (int i = 0; i < strlen(PWR_ON_MSG); i++) {
        static const int MAXCNT = 128;
        int cnt = 0;
        uint8_t c = 0x00;
        do {
            if (!recvBytes(&c, sizeof(c), 500 * 1000)) {
                return Timeout;
            }

            /*
             * What is the version of the camera.
             * You can check the version with this code.
             *
             * VC0703 1.00
             * 3o ctrl in
             * Init end
             */
#if 0
            printf("%c", c);
#endif

            cnt++;
            if (MAXCNT < cnt) {
                return UnexpectedReply;
            }
        } while (c != PWR_ON_MSG[i]);
    }
    return NoError;
}

/**
 * Send bytes to camera module.
 *
 * @param buf Pointer to the data buffer.
 * @param len Length of the data buffer.
 *
 * @return True if the data sended.
 */
bool Camera_LS_Y201::sendBytes(uint8_t *buf, size_t len, int timeout_us) {
    for (uint32_t i = 0; i < (uint32_t)len; i++) {
        int cnt = 0;
        while (!serial.writeable()) {
            wait_us(1);
            cnt++;
            if (timeout_us < cnt) {
                return false;
            }
        }
        serial.putc(buf[i]);
    }
    return true;
}

/**
 * Receive bytes from camera module.
 *
 * @param buf Pointer to the data buffer.
 * @param len Length of the data buffer.
 *
 * @return True if the data received.
 */
bool Camera_LS_Y201::recvBytes(uint8_t *buf, size_t len, int timeout_us) {
    for (uint32_t i = 0; i < (uint32_t)len; i++) {
        int cnt = 0;
        while (!serial.readable()) {
            wait_us(1);
            cnt++;
            if (timeout_us < cnt) {
                return false;
            }
        }
        buf[i] = serial.getc();
    }
    return true;
}

/**
 * Wait received.
 *
 * @return True if the data received.
 */
bool Camera_LS_Y201::waitRecv() {
    while (!serial.readable()) {
    }
    return true;
}

/**
 * Wait idle state.
 */
bool Camera_LS_Y201::waitIdle() {
    while (serial.readable()) {
        serial.getc();
    }
    return true;
}
