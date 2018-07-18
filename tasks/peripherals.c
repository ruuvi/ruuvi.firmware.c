#include "adc.h"
#include "spi.h"
#include "timer.h"
#include "pin_interrupt.h"
#include "gpio.h"
#include "yield.h"
#include "boards.h"
#include "communication.h"
#include "nfc_tag.h"
#include "nrf_crypto.h"

static uint8_t text[] = {'d', 'a', 't', 'a'};
static size_t text_len = sizeof(text);
static uint8_t url[] = {'r', 'u', 'u', 'v', 'i', '.', 'c', 'o', 'm'};
static size_t url_len = sizeof(url);
static ruuvi_communication_channel_t m_nfc;

ruuvi_status_t task_init_peripherals(void)
{
  // Buses
  ruuvi_status_t err_code = spi_init();
  
  // Setup constant records for NFC
  err_code  = nfc_text_record_set(text, text_len);
  err_code |= nfc_uri_record_set(url, url_len);

  //init NFC
  err_code |= nfc_init(&m_nfc);
  err_code |= m_nfc.process_asynchronous();

  // Timers
  err_code |= platform_timers_init();

  // GPIO
  err_code |= platform_gpio_init();
  err_code |= platform_gpio_configure(LED_RED, RUUVI_GPIO_MODE_OUTPUT_HIGHDRIVE);
  err_code |= platform_gpio_configure(LED_GREEN, RUUVI_GPIO_MODE_OUTPUT_HIGHDRIVE);
  err_code |= platform_gpio_set(LED_RED);
  err_code |= platform_gpio_set(LED_GREEN);

  platform_pin_interrupt_init();
  // platform_pin_interrupt_enable(BUTTON_1, RUUVI_GPIO_SLOPE_HITOLO, RUUVI_GPIO_MODE_INPUT_NOPULL, button_press_handler);
  nrf_crypto_rng_context_t context;
  err_code |= nrf_crypto_init();
  err_code |= nrf_crypto_rng_init(&context, NULL);
  ruuvi_sensor_t adc;
  err_code |= adc_init(&adc);

  return err_code;
}

ruuvi_status_t task_blink_leds(uint32_t time_per_led)
{
  ruuvi_status_t err_code = RUUVI_SUCCESS;
  err_code |= platform_gpio_clear(LED_RED);
  platform_delay_ms(time_per_led);
  err_code |= platform_gpio_set(LED_RED);
  platform_delay_ms(time_per_led);
  err_code |= platform_gpio_clear(LED_GREEN);
  platform_delay_ms(time_per_led);
  err_code |= platform_gpio_set(LED_GREEN);
  platform_delay_ms(time_per_led);

  return err_code;
}

ruuvi_status_t task_nfc_process(void)
{
  // return nfc.process_asynchronous();
  return RUUVI_ERROR_NOT_IMPLEMENTED;
}