/**
 * Configuration for ruuvi.drivers.c nrf5 sdk15 sdk_config. Platform has default configuration, you'll need to only define the overrides.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#include "application_config.h"

#ifndef NRF5_SDK15_APPLICATION_CONFIG_H
#define NRF5_SDK15_APPLICATION_CONFIG_H
#include "app_util.h"

// Log data will be handled immidiately, use minimal buffer size of 128 bytes
#define NRF_LOG_ENABLED APPLICATION_LOG_ENABLED
#define NRF_LOG_DEFERRED 0
#if ((APPLICATION_LOG_BUFFER_SIZE & (APPLICATION_LOG_BUFFER_SIZE - 1)) || (APPLICATION_LOG_BUFFER_SIZE<128))
  #error "APPLICATION_LOG_BUFFER_SIZE must be a power of 2 and at least 128"
#else
  #define NRF_LOG_BUFSIZE              APPLICATION_LOG_BUFFER_SIZE
  #define NRF_LOG_STR_PUSH_BUFFER_SIZE APPLICATION_LOG_BUFFER_SIZE
#endif
#define NRF_LOG_ALLOW_OVERFLOW 0

// Select one backend
#define NRF_LOG_BACKEND_RTT_ENABLED  APPLICATION_LOG_BACKEND_RTT_ENABLED
#define NRF_LOG_BACKEND_UART_ENABLED APPLICATION_LOG_BACKEND_UART_ENABLED

#define NFC_T4T_HAL_ENABLED            APPLICATION_COMMUNICATION_NFC_ENABLED
#define NFC_NDEF_RECORD_ENABLED        APPLICATION_COMMUNICATION_NFC_ENABLED
#define NFC_NDEF_MSG_ENABLED           APPLICATION_COMMUNICATION_NFC_ENABLED
#define NFC_NDEF_TEXT_RECORD_ENABLED   APPLICATION_COMMUNICATION_NFC_ENABLED
#define NFC_NDEF_MSG_PARSER_ENABLED    APPLICATION_COMMUNICATION_NFC_ENABLED
#define NFC_NDEF_RECORD_PARSER_ENABLED APPLICATION_COMMUNICATION_NFC_ENABLED

// Auto retry requires initialized timer/scheduler module
#define NRF_PWR_MGMT_CONFIG_AUTO_SHUTDOWN_RETRY 0

// DC/DC functions. Power and clock must have equal IRQ priorities.
#define NRFX_POWER_ENABLED             APPLICATION_POWER_ENABLED
#define NRFX_POWER_CONFIG_IRQ_PRIORITY NRFX_CLOCK_CONFIG_IRQ_PRIORITY

#define SPI_ENABLED                        APPLICATION_SPI_ENABLED
#define SPI_INSTANCE                       0
#define SPI_IRQ_PRIORITY                   7
#define SPI_DEFAULT_CONFIG_IRQ_PRIORITY    SPI_IRQ_PRIORITY
// <0=> NRF_GPIO_PIN_NOPULL
// <1=> NRF_GPIO_PIN_PULLDOWN
// <3=> NRF_GPIO_PIN_PULLUP
#define NRF_SPI_DRV_MISO_PULLUP_CFG        0
#define SPI0_ENABLED                       APPLICATION_SPI_ENABLED
#define SPI0_USE_EASY_DMA                  0

// 0 is used by the softdevice, 1 is used by scheduler / timer
#define NRF5_SDK15_RTC_INSTANCE 2

#define BLE_DFU_ENABLED APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define BLE_DIS_ENABLED APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define BLE_NUS_ENABLED APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define NRF_SDH_BLE_VS_UUID_COUNT BLE_DFU_ENABLED + \
                                  BLE_DIS_ENABLED + \
                                  BLE_NUS_ENABLED

#define MIN_CONN_INTERVAL                MSEC_TO_UNITS(APPLICATION_GATT_CONN_INTERVAL_MIN_MS, UNIT_1_25_MS)    /**< Minimum acceptable connection interval (0.02 seconds). */
#define MAX_CONN_INTERVAL                MSEC_TO_UNITS((APPLICATION_GATT_CONN_INTERVAL_MAX_MS), (UNIT_1_25_MS))    /**< Maximum acceptable connection interval (0.40 second). */
#define SLAVE_LATENCY                    APPLICATION_GATT_CONN_SLAVE_SKIP_INTERVALS                             /**< Slave latency. */
#define CONN_SUP_TIMEOUT                 MSEC_TO_UNITS((APPLICATION_GATT_CONN_TIMEOUT_MS), (UNIT_10_MS))            /**< Connection supervisory timeout (4 seconds). */


#endif