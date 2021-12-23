// ATMega 328p specific Functions used to interact with i2c devices  
//

#ifndef TINYWEATHER_TWI_M328P_H
#define TINYWEATHER_TWI_M328P_H
#include <stdint.h>

/**
 * @brief Intialize Two-Wire
 *
 */
void twi_init(void);

/**
 * @brief 
 *
 */
void twi_wait(void);

/**
 * @brief Send the start bit
 *
 */
void twi_start(void);

/**
 * @brief Send a stop bit
 */
void twi_stop(void);

/**
 * @brief Read with Acknowledge
 */
uint8_t twi_read_ack(void);

/**
 * @brief Read with no Acknowledge
 */
uint8_t twi_read_noack(void);

/**
 * @brief Send a byte
 */
void twi_send(uint8_t data);
#endif //TINYWEATHER_TWI_M328P_H
