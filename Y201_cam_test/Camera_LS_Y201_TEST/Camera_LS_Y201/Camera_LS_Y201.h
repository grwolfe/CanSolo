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

#ifndef LS_Y201_H
#define LS_Y201_H

#include "mbed.h"
#include "SerialBuffered.h"

/**
 * Camera
 */
class Camera_LS_Y201 {
public:

    /**
     * Create.
     *
     * @param tx Transmitter.
     * @param rx Receiver.
     */
    Camera_LS_Y201(PinName tx, PinName rx);
    
    /**
     * Dispose.
     */
    ~Camera_LS_Y201();
    
    /**
     * Error code.
     */
    enum ErrorCode {
        NoError = 0,
        UnexpectedReply,
        Timeout,
        SendError,
        RecvError,
        InvalidArguments
    };
    
    /**
     * Image size.
     */
    enum ImageSize {
        ImageSize160x120,   /**< 160x120. */
        ImageSize320x280,   /**< 320x280. */
        ImageSize640x480    /**< 640x480. */
    };

    /**
     * Reset module.
     *
     * @return Error code.
     */
    ErrorCode reset();

    /**
     * Set image size.
     *
     * @param is Image size.
     * @return Error code.
     */
    ErrorCode setImageSize(ImageSize is);

    /**
     * Take picture.
     *
     * @return Error code.
     */
    ErrorCode takePicture();

    /**
     * Read jpeg file size.
     *
     * @param fileSize File size.
     * @return Error code.
     */
    ErrorCode readJpegFileSize(int *fileSize);

    /**
     * Read jpeg file content.
     *
     * @param func A pointer to a call back function.
     * @return Error code.
     */
    ErrorCode readJpegFileContent(void (*func)(int done, int total, uint8_t *buf, size_t siz));

    /**
     * Stop taking pictures.
     *
     * @return Error code.
     */
    ErrorCode stopTakingPictures();

private:
    SerialBuffered serial;

    /**
     * Wait init end codes.
     *
     * @return Error code.
     */
    ErrorCode waitInitEnd();

    /**
     * Send bytes to camera module.
     *
     * @param buf Pointer to the data buffer.
     * @param len Length of the data buffer.
     *
     * @return True if the data sended.
     */
    bool sendBytes(uint8_t *buf, size_t len, int timeout_us);

    /**
     * Receive bytes from camera module.
     *
     * @param buf Pointer to the data buffer.
     * @param len Length of the data buffer.
     *
     * @return True if the data received.
     */
    bool recvBytes(uint8_t *buf, size_t len, int timeout_us);

    /**
     * Wait received.
     *
     * @return True if the data received.
     */
    bool waitRecv();

    /**
     * Wait idle state.
     *
     * @return True if it succeed.
     */
    bool waitIdle();

};

#endif
