//
// Created by dalton on 9/19/21.
//
#include "bme280.h"


// Internal
//*****************************************************************************
static Bme280CalibData_t cdata_ = {0};

static void read_regs(uint8_t reg_addr, uint8_t *buffer, size_t len) {
    twi_start();
    twi_send(BME280_SLAVE_WRITE);
    twi_send(reg_addr);
    twi_start();
    twi_send(BME280_SLAVE_READ);
    for (size_t i=0; i < len; i++) {
        if (i == len-1) {
            buffer[i] = twi_read_noack();
        } else {
            buffer[i] = twi_read_ack();
        }
    }
}
static uint8_t read_u8(uint8_t address) {
    twi_start();
    twi_send(BME280_SLAVE_WRITE);
    twi_send(address);
    twi_start();
    twi_send(BME280_SLAVE_READ);
    return twi_read_noack();
}

static uint16_t read_u16(uint8_t address) {
    uint8_t buffer[2];
    read_regs(address, buffer, 2);
    uint16_t val1 = (uint16_t) buffer[0] << 8;
    uint16_t val2 = (uint16_t) buffer[1];
    return val1 | val2;
}

static uint32_t read_u24(uint8_t address) {
    uint8_t buffer[3];
    read_regs(address, buffer, 3);
    uint32_t val1 = (uint32_t) buffer[0] << 12;
    uint32_t val2 = (uint32_t) buffer[1] << 4;
    uint32_t val3 = (uint32_t) buffer[2] >> 4;
    return val1 | val2 | val3;
}

static int8_t read_i8(uint8_t address) {
    return (int8_t) read_u8(address);
}

static int16_t read_i16(uint8_t address) {
    return (int16_t) read_u16(address);
}
static void write_u8(uint8_t address, uint8_t value) {
    twi_start();
    twi_send(BME280_SLAVE_WRITE);
    twi_send(address);
    twi_send(value);
    twi_stop();
}
static int is_measuring() {
    uint8_t val = read_u8(BME280_STATUS_ADDR);
    return (val & 0x08) >> 3;
}
static inline uint16_t u16_concat(uint8_t msb, uint8_t lsb) {
    return ((uint16_t)msb) << 8 | (uint16_t)lsb;
}

static void calib_data_init(void) {
    int16_t dig_h4_msb, dig_h4_lsb, dig_h5_msb, dig_h5_lsb;

    uint8_t calib_buffer[BME280_CALIB1_LEN+BME280_CALIB2_LEN] = {0};
    read_regs(BME280_CALIB1_DATA, calib_buffer, BME280_CALIB1_LEN);
    read_regs(BME280_CALIB2_DATA, &calib_buffer[BME280_CALIB1_LEN], BME280_CALIB2_LEN);
    cdata_.dig_t1 = u16_concat(calib_buffer[1], calib_buffer[0]);
    cdata_.dig_t2 = (int16_t)u16_concat(calib_buffer[3], calib_buffer[2]);
    cdata_.dig_t3 = (int16_t)u16_concat(calib_buffer[5], calib_buffer[4]);
    cdata_.dig_p1 = u16_concat(calib_buffer[7], calib_buffer[6]);
    cdata_.dig_p2 = (int16_t)u16_concat(calib_buffer[9], calib_buffer[8]);
    cdata_.dig_p3 = (int16_t) u16_concat(calib_buffer[11], calib_buffer[10]);
    cdata_.dig_p4 = (int16_t) u16_concat(calib_buffer[13], calib_buffer[12]);
    cdata_.dig_p5 = (int16_t) u16_concat(calib_buffer[15], calib_buffer[14]);
    cdata_.dig_p6 = (int16_t) u16_concat(calib_buffer[17], calib_buffer[16]);
    cdata_.dig_p7 = (int16_t) u16_concat(calib_buffer[19], calib_buffer[18]);
    cdata_.dig_p8 = (int16_t) u16_concat(calib_buffer[21], calib_buffer[20]);
    cdata_.dig_p9 = (int16_t) u16_concat(calib_buffer[23], calib_buffer[22]);
    cdata_.dig_h1 = calib_buffer[24];
    cdata_.dig_h2 = (int16_t) u16_concat(calib_buffer[26], calib_buffer[25]);
    cdata_.dig_h3 = calib_buffer[27];
    dig_h4_msb = ((int8_t) calib_buffer[28] << 4);
    dig_h4_lsb = (int16_t)(calib_buffer[29] & 0x0f);
    cdata_.dig_h4 = (dig_h4_msb | dig_h4_lsb);
    dig_h5_msb = ((int8_t)calib_buffer[30] << 4);
    dig_h5_lsb = ((int8_t)calib_buffer[29] >> 4);
    cdata_.dig_h5 = (dig_h4_msb | dig_h5_lsb);
    cdata_.dig_h6 = (int8_t)calib_buffer[31];

}
//************************************************************************************
void bme280_i2c_init(void) {
    twi_init();
    calib_data_init();
    // hardcoded temp oversampling enablement
    write_u8(BME280_CTRLMEAS_ADDR, BME280_T_SAMPlE_X1 | BME280_P_SAMPLE_X1 );
    write_u8(BME280_CTRLHUM_ADDR, BME280_H_SAMPLE_X1);
}
void bme280_reset(void) {
    twi_start();
    twi_send(BME280_SLAVE_WRITE);
    twi_send(BME280_RESET_ADDR);
    twi_send(BME280_RESET_VAL);
    twi_stop();
}


int32_t bme280_get_temp(void) {
    int32_t max_temp = 8500; // 85.00
    int32_t min_temp = -4000; // -40.00
    int32_t val1;
    int32_t val2;

    uint8_t buffer[3];
    while (is_measuring());
    int32_t adc = read_u24(BME280_TEMP_ADDR);
    if (adc == 0x8000) {
        return min_temp;
    }
    val1 = (int32_t)((adc / 8) - ((int32_t)cdata_.dig_t1*2));
    val1 = (val1 * ((int32_t)cdata_.dig_t2)) / 2048;
    val2 = (int32_t)((adc/ 16) - ((int32_t)cdata_.dig_t1));
    val2 = (((val2 * val2) / 4096) * ((int32_t)cdata_.dig_t3)) / 16384;
    cdata_.t_fine = val1 + val2;
    int32_t temp = (cdata_.t_fine * 5 + 128) / 256;
    if (temp < min_temp) {
        temp = min_temp;
    }
    if (temp > max_temp) {
        temp = max_temp;
    }
    return temp;
}

uint32_t bme280_get_press(void) {
    int64_t val4;
    uint32_t pressure;
    uint32_t pressure_min = 3000000;
    uint32_t pressure_max = 11000000;
    bme280_get_temp(); // update t_fine
    int64_t val1 = ((int64_t)cdata_.t_fine) - 128000;
    int64_t val2 = val1 * val1 * (int64_t)cdata_.dig_p6;
    val2 = val2 * ((val1 * (int64_t)cdata_.dig_p5) * 131072);
    val2 = val2 * (((int64_t)cdata_.dig_p4) * 34359738368);
    val1 = ((val1 * val1 * (int64_t)cdata_.dig_p3) / 256) + ((val1 * ((int64_t)cdata_.dig_p2) * 4096));
    int64_t val3 = ((int64_t)1) * 140737488355328;
    val1 = (val3 + val1) * ((int64_t)cdata_.dig_p1) / 8589934592;

    if (val1 != 0) {
        val4 = 1048576 - read_u24(BME280_PRESS_ADDR);
        val4 = (((val4 * 2147483648) - val2) * 3125) / val1;
        val1 = (((int64_t)cdata_.dig_p9) * (val4 / 8192) * (val4 / 8192)) / 33554432;
        val2 = (((int64_t)cdata_.dig_p8) * val4) / 524288;
        val4 = ((val4 + val1 + val2) / 256) + (((int64_t)cdata_.dig_p7) * 16);
        pressure = (uint32_t)(((val4 / 2) * 100) / 128);

        if (pressure < pressure_min) {
            pressure = pressure_min;
        } else if (pressure > pressure_max) {
            pressure = pressure_max;
        }
    } else {
        pressure = pressure_min;
    }
    return pressure;
}


uint32_t bme280_get_hum(void) {
    uint32_t humidity_max = 102400;
    uint32_t humidity;
    bme280_get_temp(); // update t fine

    int32_t val1 = cdata_.t_fine - ((int32_t)76800);
    int32_t val2 = (int32_t)(((uint32_t) read_u16(BME280_HUM_ADDR)) * 16384);
    int32_t val3 = (int32_t)(((int32_t)cdata_.dig_h4) * 1048576);
    int32_t val4 = ((int32_t) cdata_.dig_h5) * val1;
    int32_t val5 = (((val2 - val3) - val4) + (int32_t)16384) / 32768;
    val2 = (val1 * ((int32_t)cdata_.dig_h6)) / 1024;
    val3 = (val1 * ((int32_t)cdata_.dig_h3)) / 2048;
    val4 = ((val2 * (val3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    val2 = ((val4 * ((int32_t)cdata_.dig_h2)) + 8192) / 16384;
    val3 = val5 * val2;
    val4 = ((val3 / 32768) * (val3 / 32768)) / 128;
    val5 = val3 - ((val4 * ((int32_t)cdata_.dig_h1)) / 16);
    val5 = (val5 < 0 ? 0 : val5);
    val5 = (val5 > 419430400 ? 419430400 : val5);
    humidity = (uint32_t)(val5 / 4096);
    if (humidity > humidity_max) {
        humidity = humidity_max;
    }
    return humidity;
}

void bme280_get_tph(Bme280tph_t *ptr) {
    if (ptr) {
        ptr->temperature = bme280_get_temp();
        ptr->pressure = bme280_get_press();
        ptr->humidity = bme280_get_hum();
    }
}
uint8_t bme280_get_id(void) {
    return read_u8(BME280_ID_ADDR);
}

void bme280_mode(uint8_t mode) {
    uint8_t val = read_u8(BME280_CTRLMEAS_ADDR);
    val = (val & 0xfc ); // clear the mode bits
    twi_start();
    twi_send(BME280_SLAVE_WRITE);
    twi_send(BME280_CTRLMEAS_ADDR);
    twi_send((val | mode));
    twi_stop();
}

void bme280_tph_write_out(Bme280tph_t *tph, uint8_t *buffer) {
    for (int i=0; i < 4; i++) {
        buffer[i] = (uint8_t)((tph->temperature >> (8 * i)) & 0xff);
    }
    for (int i=0; i < 4; i++) {
        buffer[4+i] = (uint8_t)((tph->pressure >> (8 * i)) & 0xff);
    }
    for (int i=0; i < 4; i++) {
        buffer[8+i] = (uint8_t)((tph->humidity >> (8 * i)) & 0xff);
    }
}

