//
// Created by dalton on 9/2/21.
//

#include "crc16.h"
#define POLY 0x1021
uint16_t crc16(const uint8_t *buffer, size_t buffer_size)
{
    if (buffer == NULL) {
        return 0;
    }
    uint16_t crc = 0;
    while(buffer_size--) {
        uint16_t data = *buffer++;
        crc = crc ^ ((uint16_t)data << 8);
        for (uint8_t i=0; i < 8; i++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;

}