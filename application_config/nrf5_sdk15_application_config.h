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

// Log data will be handled immidiately, use minimal buffer size
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

#define NRF_FSTORAGE_ENABLED           APPLICATION_FLASH_ENABLED

// Enable GPIOTE abstarction
#define GPIOTE_ENABLED                        APPLICATION_GPIO_INTERRUPT_ENABLED
#define GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS APPLICATION_GPIO_INTERRUPT_NUMBER

// DC/DC functions. Power and clock must have equal IRQ priorities.
#define POWER_ENABLED                  APPLICATION_POWER_ENABLED
#define NRFX_POWER_ENABLED             POWER_ENABLED
#define NRFX_POWER_CONFIG_IRQ_PRIORITY NRFX_CLOCK_CONFIG_IRQ_PRIORITY
#define NRF_PWR_MGMT_ENABLED           POWER_ENABLED
// Auto retry requires initialized timer/scheduler module
#define NRF_PWR_MGMT_CONFIG_AUTO_SHUTDOWN_RETRY 0

// Enable Hardware RTCs used by the application. 
// 0 Is reserved for the softdevice.
// 1 is reserved for the application timer / scheduler
#define RTC_ENABLED       APPLICATION_RTC_MCU_ENABLED
#define NRFX_RTC_ENABLED  RTC_ENABLED
#define RTC2_ENABLED      RTC_ENABLED
#define NRFX_RTC2_ENABLED RTC2_ENABLED

// Enable software timer + scheduler
#define APP_TIMER_ENABLED              APPLICATION_TIMER_ENABLED
#define NRF_CLOCK_ENABLED              APP_TIMER_ENABLED
#define APP_TIMER_CONFIG_USE_SCHEDULER 0 //!< Do not use scheduler, allow timer to interrupt.
#define APP_SCHEDULER_ENABLED          APPLICATION_SCHEDULER_ENABLED

// Watchdog timer
#define WDT_ENABLED                    APPLICATION_WATCHDOG_ENABLED

// Enable SAADC
#define SAADC_ENABLED 1

#define I2C_ENABLED                        APPLICATION_I2C_ENABLED
#define I2C_INSTANCE                       1
#define I2C1_ENABLED                       APPLICATION_I2C_ENABLED
#define TWI_ENABLED                        I2C_ENABLED
#define TWI1_ENABLED                       I2C_ENABLED
#define TWI1_USE_EASY_DMA                  I2C_ENABLED
#define NRFX_TWI_ENABLED                   I2C_ENABLED
#define NRFX_TWIM_ENABLED                  I2C_ENABLED
#define NRFX_TWIM_ENABLED                  I2C_ENABLED
#define NRFX_TWIM1_ENABLED                 I2C_ENABLED
#define I2C_IRQ_PRIORITY                   7 //<! 2,3,6,7 are allowed. 

#define SPI_ENABLED                        APPLICATION_SPI_ENABLED
#define SPI_INSTANCE                       0 //<! Do not use same instance with I2C 
#define SPI_IRQ_PRIORITY                   7 //<! 2,3,6,7 are allowed. 
#define SPI_DEFAULT_CONFIG_IRQ_PRIORITY    SPI_IRQ_PRIORITY
// <0=> NRF_GPIO_PIN_NOPULL
// <1=> NRF_GPIO_PIN_PULLDOWN
// <3=> NRF_GPIO_PIN_PULLUP
#define NRF_SPI_DRV_MISO_PULLUP_CFG        0
#define SPI0_ENABLED                       APPLICATION_SPI_ENABLED
#define SPI0_USE_EASY_DMA                  0
#define NRFX_SPI0_ENABLED                  SPI_ENABLED

// 0 is used by the softdevice, 1 is used by scheduler / timer
#define NRF5_SDK15_RTC_INSTANCE            2

#define NRF_SDH_ENABLED      APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define NRF_SDH_BLE_ENABLED  APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define NRF_SDH_BLE_PERIPHERAL_LINK_COUNT  1
#define NRF_SDH_SOC_ENABLED  APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define NRF_BLE_GATT_ENABLED APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define NRF_BLE_QWR_ENABLED  APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define PEER_MANAGER_ENABLED APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define BLE_DFU_ENABLED      APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define BLE_DIS_ENABLED      APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define BLE_NUS_ENABLED      APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define NRF_SDH_BLE_VS_UUID_COUNT BLE_DFU_ENABLED + \
                                  BLE_DIS_ENABLED + \
                                  BLE_NUS_ENABLED

#define NRF_BLE_CONN_PARAMS_ENABLED                           APPLICATION_COMMUNICATION_BLUETOOTH_ENABLED
#define NRF_BLE_CONN_PARAMS_MAX_SLAVE_LATENCY_DEVIATION       499   //!< Magic number from Nordic example 
#define NRF_BLE_CONN_PARAMS_MAX_SUPERVISION_TIMEOUT_DEVIATION 65535 //!< Magic number from Nordic example 
/**< Minimum acceptable connection interval. At least 7.5 ms.  */
#define MIN_CONN_INTERVAL                MSEC_TO_UNITS(APPLICATION_GATT_CONN_INTERVAL_MIN_MS, UNIT_1_25_MS)     
 /**< Maximum acceptable connection interval.  */
#define MAX_CONN_INTERVAL                MSEC_TO_UNITS((APPLICATION_GATT_CONN_INTERVAL_MAX_MS), (UNIT_1_25_MS))
/**< How many intervals slave is allowed to skip */
#define SLAVE_LATENCY                    APPLICATION_GATT_CONN_SLAVE_SKIP_INTERVALS
/**< Timeout if no connection event in this duration. At least a few expected connection events at max interval * slave latency */                             
#define CONN_SUP_TIMEOUT                 MSEC_TO_UNITS((APPLICATION_GATT_CONN_TIMEOUT_MS), (UNIT_10_MS))        

#define FDS_ENABLED       APPLICATION_FLASH_ENABLED
#define FDS_VIRTUAL_PAGES APPLICATION_FLASH_DATA_PAGES_NUMBER

#endif