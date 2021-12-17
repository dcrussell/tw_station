//
// Created by dalton on 8/16/21.
//

#include "serial.h"
#include <math.h>
#include <stdint.h>
#include <util/setbaud.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))


static uint8_t rx_buf_[SERIAL_RX_BUFFER_SIZE];
static uint8_t tx_buf_[SERIAL_TX_BUFFER_SIZE];
static uint8_t rx_buf_head_ = 0;
static uint8_t rx_buf_tail_ = 0;
static uint8_t tx_buf_head_ = 0;
static uint8_t tx_buf_tail_ = 0;

void tx_udre_irq_() {
    uint8_t byte = tx_buf_[tx_buf_tail_];
    tx_buf_tail_ = (tx_buf_tail_ + 1) % SERIAL_TX_BUFFER_SIZE;

    UDR0 = byte;

    //TODO: Do I need to clear the transmit complete bit?
    
    // Disable interrupt if empty 
    if (tx_buf_head_ == tx_buf_tail_) {
        cbi(UCSR0B, UDRIE0);
    }

}

void serial_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1<< U2X0);
#endif
    UCSR0B = (1<< TXEN0) | ( 1 << RXEN0) | ( 1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    cbi(UCSR0B, UDRIE0);

}

// Writing for testing purposes
void serial_write_(uint8_t byte) {
    // wait for the data register to clear
    loop_until_bit_is_set(UCSR0A, UDRE0);
    cli();
    UDR0 = byte;

    // enable interrupts
    sei();
}

// Read a byte from the buffer
int  serial_read() {
    if (rx_buf_head_ == rx_buf_tail_) {
        return -1;
    } else {
        uint8_t c = rx_buf_[rx_buf_tail_];
        rx_buf_tail_ = (rx_buf_tail_ + 1) % SERIAL_RX_BUFFER_SIZE;
        return c;
    }

}

int serial_read_bytes(uint8_t *buffer, size_t nbytes) {
    int n = 0;
    while ( n != nbytes) {
        int b = -1;
        while (b == -1) {
            b = serial_read();
        }
        buffer[n++] = b;
    }
    return n;
}

void serial_write(uint8_t byte) {
    // if buffer is empty and data register is empty, 
    // write the byte directly to the register
    if (tx_buf_head_ == tx_buf_tail_ && bit_is_set(UCSR0A, UDRE0)) {
        UDR0 = byte;
        return;
    }

    uint8_t i = (tx_buf_head_ + 1) % SERIAL_TX_BUFFER_SIZE;
    // deal with full buffer
    while(i == tx_buf_tail_) {
        if (bit_is_clear(SREG, SREG_I)) {
            if(bit_is_set(UCSR0A, UDRE0)) {
                tx_udre_irq_();
            }

        } else {
            
        }
    }
    // add byte to tx buffer 
    tx_buf_[tx_buf_head_] = byte;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        tx_buf_head_ = i;
        sbi(UCSR0B, UDRIE0);
    }
}

size_t serial_write_bytes(uint8_t *bytes, size_t nbytes) {
    size_t n = 0;
    while( n < nbytes) {
        serial_write(bytes[n++]);
    }
    return n;
}

int serial_available() {
    //TODO: Is this atomic? Will this have a chance 
    //of being interrupted
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + rx_buf_head_ - rx_buf_tail_ ))% SERIAL_RX_BUFFER_SIZE ;
}

void serial_rx_clear() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        rx_buf_head_ = 0;
        rx_buf_tail_ = 0;
    }
}

// interrupt for recieved complete
ISR(USART_RX_vect) {

    //TODO: Parity?

    uint8_t c = UDR0;
    uint8_t i = (unsigned int)(rx_buf_head_ + 1) % SERIAL_RX_BUFFER_SIZE;
    // Keep a space between the head and tail 
    // if writing causes head == tail 
    // then drop the byte
    if (i != rx_buf_tail_) {
        rx_buf_[rx_buf_head_] = c;
        rx_buf_head_ = i;
    }
}

ISR(USART_UDRE_vect) {
    tx_udre_irq_();
}
