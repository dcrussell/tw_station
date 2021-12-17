//
// Created by dalton on 9/9/21.
//

#ifndef TINYWEATHER_TWI_M328P_H
#define TINYWEATHER_TWI_M328P_H
#include <stdint.h>
void twi_init(void);
void twi_wait(void);
void twi_start(void);
void twi_stop(void);
uint8_t twi_read_ack(void);
uint8_t twi_read_noack(void);
void twi_send(uint8_t data);
#endif //TINYWEATHER_TWI_M328P_H
