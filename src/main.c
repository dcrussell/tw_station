#include "bme280.h"
#include "comm.h"
#include "serial.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include "command.h"

//TODO: Update with actual main loop.
int main(void) {
    comm_init();
    sei();
    int n = 0;
    while(1) {
        uint8_t buf[86];
        size_t n = comm_listen(buf, 86);
        
        if (n == 1 && buf[0] == CMD_REQ_TPH) {
            Bme280tph_t data = { .temperature = 10, .humidity = 10, .pressure = 10 };
            uint8_t data_buf[32];
            bme280_tph_write_out(&data, data_buf);
            comm_send(data_buf, 12);
        }

    }

}

