//
// Created by dalton on 8/19/21.
//
#include <stdint.h>
#include <stdio.h>
#include "comm.h"
#include "serial.h"
#include <string.h>
#include "crc16.h"
#define FRAME_START 0x7fU
#define FRAME_END   0xfeU

#define FRAME_TYPE_CTRL 'C'
#define FRAME_TYPE_DATA 'D'


//Control frame identifiers
#define FRAME_ACK 0x01
#define FRAME_CRC_FAIL 0x02
#define FRAME_OVERSIZE 0x03
#define FRAME_INVALID 0x04


static const uint8_t ack[7] = { FRAME_START, FRAME_TYPE_CTRL, 0x1, FRAME_ACK, 0x21, 0x10, FRAME_END };


static uint8_t frame_buffer[MAX_FRAME_SIZE];

enum { eStartField,
        eTypeField,
        eLenField,
        ePayloadField };


typedef enum {
    eNone,
    eStart,
    eType,
    eLen,
    eEnd,

} state;


static inline void frame_buffer_clear(void)
{
    memset((void *)frame_buffer, 0x00, MAX_FRAME_SIZE);
}

static inline void copy_from_frame_buffer(uint8_t *dst, size_t start, size_t n_bytes)
{
    memcpy((void *)dst, (void *)&frame_buffer[start], n_bytes);
}
static inline void copy_to_frame_buffer(uint8_t *src, size_t start, size_t n_bytes)
{
    memcpy((void *)&frame_buffer[start], (void *)src, n_bytes);
}

// Send a nack frame with a msg
static void send_nack(uint8_t nack_type)
{
    frame_buffer[eStartField] = FRAME_START;
    frame_buffer[eTypeField] = FRAME_TYPE_CTRL;
    frame_buffer[eLenField] = 1;
    frame_buffer[ePayloadField] = nack_type;
    uint16_t crc = crc16(&frame_buffer[ePayloadField], frame_buffer[eLenField]);
    frame_buffer[ePayloadField+frame_buffer[eLenField]] = crc & 0xff;
    frame_buffer[ePayloadField+frame_buffer[eLenField]+1] = (crc >> 8);
    frame_buffer[ePayloadField+frame_buffer[eLenField]+2] = FRAME_END;
    serial_write_bytes(frame_buffer, frame_buffer[eLenField]+6);
    frame_buffer_clear();
}

static void send_ack(void)
{
    serial_write_bytes(ack, 7);
    frame_buffer_clear();
}

static int check_crc(size_t frame_size)
{
    uint16_t crc = frame_buffer[frame_size-3];
    crc = (frame_buffer[frame_size-2] << 8) + crc;
    uint16_t check = crc16(&frame_buffer[ePayloadField], frame_buffer[eLenField]);
    return (check-crc);
}

size_t comm_listen(uint8_t *buffer, size_t buffer_lengh) {
    if ((buffer == NULL) || buffer_lengh == 0) {
        return 0;
    }
    size_t n_bytes = 0;
    int s = eNone;
    while ((s != eEnd) && (n_bytes < MAX_FRAME_SIZE)) {
        int byte = -1;
        while(byte == -1) {
            byte = serial_read();
        }

        switch (s) {
            case eNone: {
                if (byte == FRAME_START) {
                    frame_buffer[n_bytes++] = (uint8_t) byte;
                    s = eStart;
                } else {
                    frame_buffer_clear();
                    return 0;
                }
                break;
            }
            case eStart: {
                if (byte != FRAME_TYPE_DATA) {
                    frame_buffer_clear();
                    send_nack(FRAME_INVALID);
                    return 0;
                }
                s = eType;
                frame_buffer[n_bytes++] = (uint8_t) byte;
                break;
            }
            case eType: {
                if (byte > (MAX_FRAME_SIZE - 6))  {
                    frame_buffer_clear();
                    send_nack(FRAME_OVERSIZE);
                    return 0;
                }
                frame_buffer[n_bytes++] = (uint8_t) byte;
                s = eLen;
                break;
            }
            case eLen: {
                if (byte == FRAME_END) {
                    s = eEnd;
                }
                frame_buffer[n_bytes++] = (uint8_t) byte;
                break;
            }
        }
    }
    // CRC check failed
    if (check_crc(n_bytes) != 0) {
        frame_buffer_clear();
        send_nack(FRAME_CRC_FAIL);
        return 0;
    }
    size_t cpy_sz = frame_buffer[eLenField] > buffer_lengh ? buffer_lengh : frame_buffer[eLenField];
    copy_from_frame_buffer(buffer, ePayloadField, cpy_sz);
    frame_buffer_clear();
    send_ack();
    return cpy_sz;
}


void comm_send(uint8_t *buffer, size_t nbytes) {
    if ((buffer == NULL) || (nbytes == 0 )) {
        return;
    }
    if ((nbytes+6) > MAX_FRAME_SIZE) {
        return;
    }
    frame_buffer[eStartField] = FRAME_START;
    frame_buffer[eTypeField] = FRAME_TYPE_DATA;
    frame_buffer[eLenField] = nbytes;
    copy_to_frame_buffer(buffer, ePayloadField, nbytes);
    uint16_t crc = crc16(&frame_buffer[ePayloadField], frame_buffer[eLenField]);
    frame_buffer[ePayloadField+frame_buffer[eLenField]] = crc & 0xff;
    frame_buffer[ePayloadField+frame_buffer[eLenField]+1] = (crc >> 8);
    frame_buffer[ePayloadField+frame_buffer[eLenField]+2] = FRAME_END;
    int attempts = 0;
    while (attempts < NUM_ATTEMPTS) {
        serial_write_bytes(frame_buffer, frame_buffer[eLenField]+6);
        attempts++;
        uint8_t bytes[4] = {0x00};
        serial_read_bytes(bytes, 4);
        serial_rx_clear(); // clear the remaining bytes from the buffer
        if (bytes[3] == FRAME_ACK) {
            break;
        }
    }
    frame_buffer_clear();
}

void comm_init(void) {
    serial_init();
}
