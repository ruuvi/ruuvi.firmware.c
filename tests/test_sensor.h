/**
 * Ruuvi Firmware 3.x Tests
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#ifndef  TEST_SENSOR_H
#define  TEST_SENSOR_H

#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_log.h"

/**
 * Test that sensor init and uninit works as expected.
 *
 * - Sensor must return RUUVI_DRIVER_SUCCESS on first init.
 * - None of the sensor function pointers may be NULL after init
 * - Sensor should return RUUVI_DRIVER_ERROR_INVALID_STATE when initializing sensor which is already init. May return other error if check for it triggers first.
 * - Sensor must return RUUVI_DRIVER_SUCCESS on first uninit
 * - All of sensor function pointers must be NULL after uninit
 * - Sensor configuration is not defined after init, however the sensor must be in lowest-power state available.
 * - Sensor configuration is not defined after uninit, however the sensor must be in lowest-power state available.
 *   - Sensor power consumption is not tested by this test.
 * - Sensor initialization must be successful after uninit.
 * - Init and Uninit should return RUUVI_DRIVER_ERROR_NULL if pointer to the sensor struct is NULL. May return other error if check for it triggers first.
 *
 *
 * parameter init:   Function pointer to sensor initialization
 * parameter bus:    Bus of the sensor, RUUVI_DRIVER_BUS_NONE, _I2C or _SPI
 * parameter handle: Handle of the sensor, such as SPI GPIO pin, I2C address or ADC channel.
 *
 * return: RUUVI_DRIVER_SUCCESS if the tests passed, error code from the test otherwise.
 */
ruuvi_driver_status_t test_sensor_init(const ruuvi_driver_sensor_init_fp init, const ruuvi_driver_bus_t bus, const uint8_t handle);

/**
 *  Test that sensor sample rate, scale and resolution setters and getters work as expected.
 *
 * - MIN, MAX, DEFAULT and NO_CHANGE must always return RUUVI_DRIVER_SUCCESS
 * - On any parameter set between 1 and 200, if driver returns SUCCESS, the returned value must be at least as much as what was set.
 * - GET must return same value as SET had as an output.
 * - Get and Set should return RUUVI_DRIVER_ERROR_NULL if pointer to the value is NULL. May return other error if check for it triggers first.
 *
 */
ruuvi_driver_status_t test_sensor_setup(const ruuvi_driver_sensor_init_fp init, const ruuvi_driver_bus_t bus, const uint8_t handle);

/**
 * Get total number of tests run and total number of tests passed.
 *
 * parameter total:   pointer to value which will be set to the number of tests run
 * parameter passedl: pointer to value which will be set to the number of tests passed
 *
 * return RUUVI_DRIVER_SUCCESS
 */
ruuvi_driver_status_t test_sensor_status(size_t* total, size_t* passed);

/**
 * Run all tests
 * Any errors get logged during the test.
 * Results can be read with test_sensor_status
 */
void test_sensor_run(void);

#endif