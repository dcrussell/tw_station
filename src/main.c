#include "bme280.h"
#include "comm.h"
#include "serial.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include "command.h"
#include <util/delay.h>
//TODO: Update with actual main loop.
int main(void) {
    comm_init();
    bme280_i2c_init();
    sei();
    while(1) {
        uint8_t cmd[1] = {0};
        uint8_t data[12] = {0};
        while (!comm_listen(cmd, 1)) {}
        switch (cmd[0]) {
            case CMD_REQ_TPH:
                {
                    Bme280tph_t tph;
                    Bme280tph_t *tph_ptr = &tph;
                    bme280_mode(BME280_MODE_FORCED);
                    bme280_get_tph(tph_ptr);
                    bme280_tph_write_out(tph_ptr, data);
                    comm_send(data, 12);
                    break;

                }
            case CMD_REQ_T:
                {
                    bme280_mode(0x01);
                    int32_t temp = bme280_get_temp();
                    for (int i=0; i<4; i++) {
                        data[i] = (uint8_t)((temp >> (8 * i)) & 0xff);
                    }
                    comm_send(data, 4);
                    break;

                }
            case CMD_REQ_H:
                {
                    bme280_mode(BME280_MODE_FORCED);
                    uint32_t hum = bme280_get_hum();
                    for (int i=0; i<4; i++) {
                        data[i] = (uint8_t)((hum >> (8 * i)) & 0xff);
                    }
                    comm_send(data, 4);
                    break;

                }
            case CMD_REQ_P:
                {
                    bme280_mode(BME280_MODE_FORCED);
                    uint32_t press = bme280_get_press();
                    for (int i=0; i<4; i++) {
                        data[i] = (uint8_t)((press >> (8 * i)) & 0xff);
                    }
                    comm_send(data, 4);
                    break;
                }
            default:
                break;
        }
    }
        

}

