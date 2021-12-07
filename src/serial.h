//
// Created by dalton on 8/16/21.
//

#ifndef TWSTATION_SERIAL_H
#define TWSTATION_SERIAL_H
#include <stddef.h>
#include <stdint.h>

#define SERIAL_RXTX_SIZE_DEFAULT 32 

#ifndef SERIAL_RX_BUFFER_SIZE
#define SERIAL_RX_BUFFER_SIZE SERIAL_RXTX_SIZE_DEFAULT
#endif 

#ifndef SERIAL_TX_BUFFER_SIZE 
#define SERIAL_TX_BUFFER_SIZE SERIAL_RXTX_SIZE_DEFAULT
#endif

#if (SERIAL_RX_BUFFER_SIZE>256)
#error "Serial buffer size cannot be greater than 256"
#endif 

#if (SERIAL_TX_BUFFER_SIZE>256)
#error "Serial buffer size cannot be greater than 256"
#endif 

#define SERIAL_BAUD_DEFULT 9600

#ifndef BAUD
#define BAUD SERIAL_BAUD_DEFULT 
#endif 


/*
 * @brief Get the number of bytes available to be read 
 *
 *
 * @return The number of bytes available for reading
 */
int serial_available(void);

/*
 * Write bytes 
 */
void serial_write(uint8_t byte);
size_t serial_write_bytes(uint8_t *bytes, size_t nbytes);

/** 
 *
 */ 
int serial_read_bytes(uint8_t *buffer, size_t buffer_size);

void serial_write_(uint8_t byte);

/*
 *
 */
int serial_read();

void serial_rx_clear();

/*
 * Initialize the serial 
 */
void serial_init();


#endif //TWSTATION_SERIAL_H 
