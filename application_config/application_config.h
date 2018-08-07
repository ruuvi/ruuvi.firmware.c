/**
 * Application configuration constants
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H

#define APPLICATION_GPIO_ENABLED    1
#define APPLICATION_GPIO_INTERRUPT_ENABLED 1
#define APPLICATION_YIELD_ENABLED   1

#define APPLICATION_LOG_ENABLED 1
#define APPLICATION_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#define APPLICATION_LOG_BACKEND_RTT_ENABLED 1
#define APPLICATION_LOG_BACKEND_UART_ENABLED 0

// Pick a power of 2 for nRF5 backed. 128 is recommended.
#define APPLICATION_LOG_BUFFER_SIZE 128



// Use nRF5 SDK15
#define NRF5_SDK15_PLATFORM_ENABLED 1

#endif