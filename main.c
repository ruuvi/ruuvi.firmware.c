/**
 * Ruuvi Firmware 3.x code. Reads the sensors onboard RuuviTag and broadcasts the sensor data in a manufacturer specific format.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_boards.h"

int main(void)
{
  ruuvi_platform_yield_init();

  // Init GPIO
  ruuvi_platform_gpio_init();

  // Turn off sensors∫
  ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_SS_ACCELERATION_PIN,  RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_SS_ACCELERATION_PIN,  RUUVI_INTERFACE_GPIO_HIGH);
  ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN, RUUVI_INTERFACE_GPIO_HIGH);

  // Put SPI lines into HIGH state to avoid power leaks
  ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_SCK_PIN,  RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_SCK_PIN,  RUUVI_INTERFACE_GPIO_HIGH);
  ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_MOSI_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_write    (RUUVI_BOARD_SPI_MOSI_PIN, RUUVI_INTERFACE_GPIO_HIGH);

  // LEDs high / inactive
  ruuvi_platform_gpio_configure(RUUVI_BOARD_LED_RED,   RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE);
  ruuvi_platform_gpio_write    (RUUVI_BOARD_LED_RED,   RUUVI_INTERFACE_GPIO_HIGH);
  ruuvi_platform_gpio_configure(RUUVI_BOARD_LED_GREEN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE);
  ruuvi_platform_gpio_write    (RUUVI_BOARD_LED_GREEN, RUUVI_INTERFACE_GPIO_HIGH);

  // Button, and SPI MISO lines pulled up
  ruuvi_platform_gpio_configure(RUUVI_BOARD_BUTTON_1,     RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP);
  ruuvi_platform_gpio_configure(RUUVI_BOARD_SPI_MISO_PIN, RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP);

  // Interrupt lines are driven by LIS2DH12
//  ruuvi_platform_gpio_configure(RUUVI_BOARD_INT_ACC1_PIN, RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP);
//  ruuvi_platform_gpio_configure(RUUVI_BOARD_INT_ACC2_PIN, RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP);

  while (1)
  {
    ruuvi_platform_yield();
  }
}