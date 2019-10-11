/**
 * @file task_spi.h
 * @author Otso Jousimaa
 * @date 2019-06-27
 * @brief SPI bus related tasks.
 * @copyright Copyright 2019 Ruuvi Innovations.
 *   This project is released under the BSD-3-Clause License.
 *
 */
#ifndef  TASK_SPI_H
#define  TASK_SPI_H

#include "ruuvi_driver_error.h"

/**
 * @brief Initialize SPI bus
 *
 * This function reads SPI pin definitions from board definition and
 * initializes the SPI data pins and chip select pins as well as the SPI peripheral.
 *
 * @return RUUVI_DRIVER_SUCCESS
 */
ruuvi_driver_status_t task_spi_init(void);


#endif