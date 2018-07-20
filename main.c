#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_yield.h"
#include "ruuvitag_b.h"

int main(void)
{
  ruuvi_driver_status_t err_code = ruuvi_platform_yield_init();

  // Init GPIO
  ruuvi_platform_gpio_init();

  // Turn off sensors
  ruuvi_platform_gpio_configure(SPIM0_SS_ACCELERATION_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_set(SPIM0_SS_ACCELERATION_PIN);
  ruuvi_platform_gpio_configure(SPIM0_SS_ENVIRONMENTAL_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_set(SPIM0_SS_ENVIRONMENTAL_PIN);
  ruuvi_platform_gpio_configure(SPIM0_SCK_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_set(SPIM0_SCK_PIN);
  ruuvi_platform_gpio_configure(SPIM0_MOSI_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_set(SPIM0_MOSI_PIN);
  ruuvi_platform_gpio_configure(SPIM0_MISO_PIN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_set(SPIM0_MISO_PIN);

  // LEDs high
  ruuvi_platform_gpio_configure(LED_RED, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_set(LED_RED);
  ruuvi_platform_gpio_configure(LED_GREEN, RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
  ruuvi_platform_gpio_set(LED_GREEN);

  // Button up
  ruuvi_platform_gpio_configure(BUTTON_1, RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP);

  while (1)
  {
    ruuvi_platform_yield();
  }
}