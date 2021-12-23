// Module provides functions for sending and recieving data over USART using 
// a custom transport protocol for (quasi)reliable delivery.
#ifndef TWSTATION_COMM_H
#define TWSTATION_COMM_H
#include <stddef.h>
#include <stdint.h>

#ifndef MAX_FRAME_SIZE
#define MAX_FRAME_SIZE 86
#endif

#ifndef NUM_ATTEMPTS
#define NUM_ATTEMPTS 1 //TODO: Change back
#endif
        
/**
 * @brief Initialize the communications. 
 *
 */
void comm_init(void);
/**
 * @brief Wait for a frame to be received and return its payload via the buffer pointer.
 * @param buffer The buffer to write the frame payload to.
 * @param buffer_size The size of the buffer.
 * @return The number of bytes written to the buffer.
 */
size_t comm_recv(uint8_t *buffer, size_t buffer_size);

/**
 * @brief Listen for incoming data.
 * @param buffer The buffer 
 *
 */
size_t comm_listen(uint8_t *buffer, size_t buffer_size);

/**
 * @brief Send a buffer of data.
 * @param buffer The data be sent.
 * @param buffer_size The size of the data to be sent.
 */

void comm_send(uint8_t *buffer, size_t nbytes);

#endif //TWSTATION_COMM_H
