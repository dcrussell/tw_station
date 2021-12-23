// Utility function providing CRC16 calculation
#ifndef TINYWEATHER_CRC16_H
#define TINYWEATHER_CRC16_H
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Calculate the CRC16-XMODEM for a buffer 
 * @param buffer Pointer to the buffer
 * @param buffer_size Size of the buffer 
 *
 * @return The calculated value.
 */
uint16_t crc16(const uint8_t *buffer, size_t buffer_size);
#endif //TINYWEATHER_CRC16_H
