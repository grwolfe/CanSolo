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

#ifndef _SERIAL_BUFFERED_H_
#define _SERIAL_BUFFERED_H_

/**
 * Buffered serial class.
 */
class SerialBuffered : public Serial {
public:
    /**
     * Create a buffered serial class.
     *
     * @param tx A pin for transmit.
     * @param rx A pin for receive.
     */
    SerialBuffered(PinName tx, PinName rx);

    /**
     * Destroy.
     */
    virtual ~SerialBuffered();

    /**
     * Get a character.
     *
     * @return A character. (-1:timeout)
     */
    int getc();

    /**
     * Returns 1 if there is a character available to read, 0 otherwise.
     */
    int readable();

    /**
     * Set timeout for getc().
     *
     * @param ms milliseconds. (-1:Disable timeout)
     */
    void setTimeout(int ms);

    /**
     * Read requested bytes.
     *
     * @param bytes A pointer to a buffer.
     * @param requested Length.
     *
     * @return Readed byte length.
     */
    size_t readBytes(uint8_t *bytes, size_t requested);

private:
    void handleInterrupt();
    static const int BUFFERSIZE = 4096;
    uint8_t buffer[BUFFERSIZE];            // points at a circular buffer, containing data from m_contentStart, for m_contentSize bytes, wrapping when you get to the end
    uint16_t indexContentStart;   // index of first bytes of content
    uint16_t indexContentEnd;     // index of bytes after last byte of content
    int timeout;
    Timer timer;
};

#endif
