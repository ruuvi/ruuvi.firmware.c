#include "tasks.h"
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
static ruuvi_communication_channel_t nfc;

ruuvi_status_t task_init_peripherals(void)
{
  // Comms
  ruuvi_status_t err_code = spi_init();

  // Functions for implementing communication api

  nfc.init = nfc_init;
  nfc.uninit = nfc_uninit;
  nfc.message_get = nfc_message_get;
  nfc.process_asynchronous = nfc_process_asynchronous;

  // Setup constant records
  err_code = nfc_text_record_set(text, text_len);
  err_code |= nfc_uri_record_set(url, url_len);

  //init
  err_code |= nfc.init();
  err_code |= nfc.process_asynchronous();

  // Timers
  err_code |= platform_timers_init();

  // GPIO
  err_code |= platform_gpio_init();
  err_code |= platform_gpio_configure(LED_RED, RUUVI_GPIO_MODE_OUTPUT_HIGHDRIVE);
  err_code |= platform_gpio_configure(LED_GREEN, RUUVI_GPIO_MODE_OUTPUT_HIGHDRIVE);
  err_code |= platform_gpio_set(LED_RED);
  err_code |= platform_gpio_set(LED_GREEN);

  platform_pin_interrupt_init();
  //platform_pin_interrupt_enable(BUTTON_1, RUUVI_GPIO_SLOPE_HITOLO, RUUVI_GPIO_MODE_INPUT_NOPULL, button_press_handler);
  nrf_crypto_rng_context_t context;
  err_code |= nrf_crypto_init();
  err_code |= nrf_crypto_rng_init(&context, NULL);

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
  return nfc.process_asynchronous();
}