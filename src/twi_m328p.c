//
// Created by dalton on 9/9/21.
//

#include "twi_m328p.h"
#include <avr/io.h>
void twi_init(void)
{
    TWBR = 32;
    TWCR |= (1 << TWEN);

}

void twi_wait(void)
{
    loop_until_bit_is_set(TWCR, TWINT);
}

void twi_start(void)
{
    TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWSTA));
    twi_wait();
}

void twi_stop(void)
{
    TWCR = ( _BV(TWINT) | _BV(TWEN) | _BV(TWSTO));
}

uint8_t twi_read_ack(void)
{
    TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWEA));
    twi_wait();
    return (TWDR);
}

uint8_t twi_read_noack(void)
{
    TWCR = (_BV(TWINT) | _BV(TWEN));
    twi_wait();
    return (TWDR);
}

void twi_send(uint8_t data)
{
    TWDR = data;
    TWCR = (_BV(TWINT) | _BV(TWEN));
    twi_wait();
}