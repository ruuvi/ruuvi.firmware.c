#ifndef APPLICATION_DRIVER_CONFIGURATION_H
#define APPLICATION_DRIVER_CONFIGURATION_H

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_platform_nrf5_sdk15_config.h"
#include "nrf5_sdk15_application_config.h"

/** @brief enable compiling environmental functions */
#define RUUVI_INTERFACE_ENVIRONMENTAL_ENABLED            APPLICATION_ENVIRONMENTAL_ENABLED

/** @brief Enable compiling BME280 interface functions */
#define RUUVI_INTERFACE_ENVIRONMENTAL_BME280_ENABLED     APPLICATION_ENVIRONMENTAL_BME280_ENABLED

/** @brief Enable compiling BME280 SPI interface */
#define RUUVI_INTERFACE_ENVIRONMENTAL_BME280_SPI_ENABLED (RUUVI_BOARD_ENVIRONMENTAL_BME280_SPI_USE && APPLICATION_ENVIRONMENTAL_BME280_ENABLED)

/** @brief Enable compiling BME280 I2C interface */
#define RUUVI_INTERFACE_ENVIRONMENTAL_BME280_I2C_ENABLED (RUUVI_BOARD_ENVIRONMENTAL_BME280_I2C_USE && APPLICATION_ENVIRONMENTAL_BME280_ENABLED)

/** @brief Enable compiling SHTCX interface functions */
#define RUUVI_INTERFACE_ENVIRONMENTAL_SHTCX_ENABLED      APPLICATION_ENVIRONMENTAL_SHTCX_ENABLED

/** @brief Enable compiling MCU built-in environmental functions */
#define RUUVI_INTERFACE_ENVIRONMENTAL_MCU_ENABLED        APPLICATION_ENVIRONMENTAL_MCU_ENABLED

/** @brief Enable compiling MCU built-in environmental functions */
#define RUUVI_INTERFACE_ENVIRONMENTAL_TMP117_ENABLED     APPLICATION_ENVIRONMENTAL_TMP117_ENABLED

#endif