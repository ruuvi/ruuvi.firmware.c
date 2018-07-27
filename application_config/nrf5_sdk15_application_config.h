/**
 * Configuration for ruuvi.drivers.c nrf5 sdk15 sdk_config. Platform has default configuration, you'll need to only define the overrides.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#include "application_config.h"

#ifndef NRF5_SDK15_APPLICATION_CONFIG_H
#define NRF5_SDK15_APPLICATION_CONFIG_H 

// Log data will be handled immidiately, use minimal buffer size of 128 bytes
#define NRF_LOG_ENABLED APPLICATION_LOG_ENABLED
#define NRF_LOG_DEFERRED 0
#define NRF_LOG_BUFSIZE APPLICATION_LOG_BUFFER_SIZE
#define NRF_LOG_ALLOW_OVERFLOW 0

// Select one backend
#define NRF_LOG_BACKEND_RTT_ENABLED 1
#define NRF_LOG_BACKEND_UART_ENABLED 0

#endif