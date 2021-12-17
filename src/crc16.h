//
// Created by dalton on 9/2/21.
//

#ifndef TINYWEATHER_CRC16_H
#define TINYWEATHER_CRC16_H
#include <stddef.h>
#include <stdint.h>
uint16_t crc16(const uint8_t *buffer, size_t buffer_size);
#endif //TINYWEATHER_CRC16_H
