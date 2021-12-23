// BME280 sensor driver. Utilizes the Two-Wire (i2c) interface 
// of the ATMega 328p 
#ifndef TINYWEATHER_BME280_H
#define TINYWEATHER_BME280_H
#include <stdint.h>
#include <unistd.h>
#include "twi_m328p.h"

// The slave address of the device.
// The alternate address is the case if the SDO
// pin of the device is pulled high.

#ifndef BME280_SLAVE_ALT
#define BME280_SLAVE_ADDR       0x77U
#else
#define BME280_SLAVE_ADDR       0x77U
#endif

#define BME280_SLAVE_WRITE      ((BME280_SLAVE_ADDR << 1))
#define BME280_SLAVE_READ       (((BME280_SLAVE_ADDR << 1) | 1))


// Register macros
#define BME280_ID_ADDR          0xD0U // Chip ID register
#define BME280_RESET_ADDR       0xE0U // Reset register

// Humidity control register sets the humidity
// data acquisiton options for the device.
// Changes only take effect after a write operation to
// CTRLMEAS register
#define BME280_CTRLHUM_ADDR     0xF2U
#define BME280_H_SAMPLE_X1      1
#define BME280_H_SAMPLE_X2      2
#define BME280_H_SAMPLE_X4      3
#define BME280_H_SAMPLE_X8      4

// Status register. Contains two bits which indicates
// the status of the device.
// bit 3: set to 1 when a conversion takes place. 0 when
// the results have been transfered to the data register.
// bit 0:
#define BME280_STATUS_ADDR      0xF3U

// Measure control register. Sets the temperature and
// data acquisition options for the device.
// Needs to be written after the Humidity control register
// for the changes to take effect.
#define BME280_CTRLMEAS_ADDR    0xF4U
#define BME280_T_SAMPlE_X1      (1 << 5)
#define BME280_T_SAMPLE_X2      (2 << 5)
#define BME280_T_SAMPLE_X4      (3 << 5)
#define BME280_T_SAMPLE_X8      (4 << 5)
#define BME280_P_SAMPLE_X1      (1 << 2)
#define BME280_P_SAMPLE_X2      (2 << 2)
#define BME280_P_SAMPLE_X4      (3 << 2)
#define BME280_P_SAMPLE_X8      (4 << 2)

// Sets the rate, filter, and interface options
// for the device.
#define BME280_CONFIG_ADDR      0xF5U

// Contains the raw pressure data in F7...F9
#define BME280_PRESS_ADDR       0xF7U

// contains the raw temperature data in FA...FC.
//
#define BME280_TEMP_ADDR        0xFAU

// Contains the raw humidity data in FD..FE
#define BME280_HUM_ADDR         0xFDU

// The first bank of calibration data
#define BME280_CALIB1_DATA      0x88U
#define BME280_CALIB1_LEN       25

// second bank of calibration data
#define BME280_CALIB2_DATA      0xE1
#define BME280_CALIB2_LEN       15


// Reset value and measuring modes
#define BME280_RESET_VAL        0xB6U
#define BME280_MODE_SLEEP       0x00U
#define BME280_MODE_FORCED      0x01U
#define BME280_MODE_NORMAL      0x03U


typedef struct {
    int32_t temperature;
    uint32_t pressure;
    uint32_t humidity;
} Bme280tph_t;


/**
 * @brief Initialize the sensor and start the i2c interface
 */
void bme280_i2c_init(void);

// TODO: finish normal mode description.
/**
 * @brief Set the mode for taking measurements
 *      0x00            - Sleep mode. Lowest power, no measurements are taken.
 *      0x01 or 0x02    - Forced mode. Measurements are taken when either of these values are
 *                        set.
 *      0x03            - Normal Mode. Measurements are periodically taken.
 * @param mode Mode to set.
 */
void bme280_mode(uint8_t mode);

/**
 * @brief Get current readings for the temperature, pressure, and humidity.
 * @param ptr Pointer to the Bme280tph_t struct to place the values.
 */
void bme280_get_tph(Bme280tph_t *ptr);


/**
 * @brief Get temperature data from the sensor
 *
 * Mode must be set before calling. If desired
 * mode is forced, then the mode must be set
 * before right before the call for temperature. Otherwise,
 * the data may be old or non-existant.
 *
 * @return The temperature data
 */
int32_t bme280_get_temp(void);

/**
 * @brief Get pressure data from the sensor.
 *
 * Mode must be set before calling. If desired
 * mode is forced, then the mode must be set
 * before right before the call for pressure. Otherwise,
 * the data may be old or non-existant.
 *
 * @return Pressure data
 */
uint32_t bme280_get_press(void);

/**
 * @brief Get the humidity data from the sensor
 *
 * Mode must be set before calling. If desired
 * mode is forced, then the mode must be set
 * before right before the call for humidity. Otherwise,
 * the data may be old or non-existant.
 *
 * @return Humidity data.
 */
uint32_t bme280_get_hum(void);

/**
 * @brief Get the ID of the sensor
 * @return The sensor's ID
 */
uint8_t bme280_get_id(void);

/**
 * @brief Reset the sensor to it's default state.
 *
 */
void bme280_reset(void);

/**
 * @brief Write out the tph struct to a buffer.
 *
 * @param tph
 * @param buffer
 */
void bme280_tph_write_out(Bme280tph_t *tph, uint8_t *buffer);

#endif //TINYWEATHER_BME280_H
