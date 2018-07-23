/**
 * Configuration for ruuvi.drivers.c nrf5 sdk15 platform support. Enable or disable modules you're using here.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#define NRF5_SDK15_PLATFORM_ENABLED 1
#if NRF5_SDK15_PLATFORM_ENABLED
#define NRF5_SDK15_GPIO_ENABLED 1
#define NRF5_SDK15_YIELD_ENABLED 1
#endif