// Functions for sending and recieving data using the 
// USART interface
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


/**
 * @brief Get the number of bytes available to be read 
 *
 */
int serial_available(void);

/**
 * @brief Write a byte via USART
 */
void serial_write(uint8_t byte);

/**
 * @brief Write a buffer of bytes via USART 
 *
 * @param bytes Pointer to the bytes to write 
 * @param nbytes Number of bytes to write
 *
 * @return Number of bytes that were actually written
 *
 */
size_t serial_write_bytes(uint8_t *bytes, size_t nbytes);

/**
 * @brief Read Bytes into a buffer,
 *
 * Buffer must be atleast nbytes in size
 * 
 * @param buffer Pointer to the buffer to read the bytes into 
 * @param nbytes Number of bytes to read 
 *
 * @return The number of bytes actually read.
 */ 
int serial_read_bytes(uint8_t *buffer, size_t nbytes);

/*
 * @brief Read a byte via USART 
 *
 * @return The byte read or -1 if no byte is available
 */
int serial_read();


/**
 * @brief Clear out internal RX buffer
 */
void serial_rx_clear();

/*
 * @brief Intitialize USART 
 */
void serial_init();


#endif //TWSTATION_SERIAL_H 
