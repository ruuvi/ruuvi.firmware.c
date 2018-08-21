/**
 * Application configuration constants
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H

#define APPLICATION_GPIO_ENABLED                 1
#define APPLICATION_GPIO_INTERRUPT_ENABLED       1
#define APPLICATION_ENVIRONMENTAL_MCU_ENABLED    1
#define APPLICATION_ENVIRONMENTAL_BME280_ENABLED 1
#define APPLICATION_SPI_ENABLED                  1
#define APPLICATION_YIELD_ENABLED                1

#define APPLICATION_LOG_ENABLED                  1
#define APPLICATION_LOG_LEVEL                    RUUVI_INTERFACE_LOG_INFO
#define APPLICATION_LOG_BACKEND_RTT_ENABLED      1
#define APPLICATION_LOG_BACKEND_UART_ENABLED     0

// Pick a power of 2 for nRF5 backed. 128 is recommended.
#define APPLICATION_LOG_BUFFER_SIZE              128

// Use nRF5 SDK15
#define NRF5_SDK15_PLATFORM_ENABLED              1

/**
 * Environmental sensor configuration
 **/
// Sample rate is in Hz. This configures only the sensor, not transmission rate of data.
#define APPLICATION_ENVIRONMENTAL_SAMPLERATE 1

// Resolution and scale cannot be adjusted
#define APPLICATION_ENVIRONMENTAL_RESOLUTION RUUVI_DRIVER_SENSOR_CFG_DEFAULT
#define APPLICATION_ENVIRONMENTAL_SCALE      RUUVI_DRIVER_SENSOR_CFG_DEFAULT

// Valid values for BME280 are: (RUUVI_DRIVER_SENSOR_DSP_)LAST, IIR, OS
// IIR slows step response but lowers noise
// OS increases power consumption but lowers noise.
// See https://blog.ruuvi.com/humidity-sensor-673c5b7636fc and https://blog.ruuvi.com/dsp-compromises-3f264a6b6344
#define APPLICATION_ENVIRONMENTAL_DSPFUNC    RUUVI_DRIVER_SENSOR_DSP_OS

// No effect on _LAST, use 1. On _OS and _IIR valid values are 2, 4, 8 and 16.
#define APPLICATION_ENVIRONMENTAL_DSPPARAM   RUUVI_DRIVER_SENSOR_CFG_MAX

// (RUUVI_DRIVER_SENSOR_CFG_)SLEEP, SINGLE or CONTINUOUS
#define APPLICATION_ENVIRONMENTAL_MODE       RUUVI_DRIVER_SENSOR_CFG_SINGLE

// Allow BME280 support compilation
#define RUUVI_INTERFACE_ENVIRONMENTAL_BME280_ENABLED 1

// Allow using MCU as environmental (temperature) sensor
#define APPLICATION_ENVIRONMENTAL_MCU_ENABLED 1


// Allow LIS2DH12 support compilation
#define RUUVI_INTERFACE_ACCELERATION_LIS2DH12_ENABLED 1


#endif