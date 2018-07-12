#include <string.h>
#include "ruuvi_endpoints.h"
#include "task_led.h"
#include "boards.h"
#include "gpio.h"

// Initialize relevant GPIO pins as standard drive outputs.
// Deactivate leds
ruuvi_status_t task_led_init(void)
{
  ruuvi_status_t err_code = RUUVI_SUCCESS;
  err_code |= platform_gpio_init();
  uint8_t leds[] = LEDS_LIST;
  for(size_t ii = 0; ii < LEDS_NUMBER; ii++)
  {
    platform_gpio_configure(leds[ii], RUUVI_GPIO_MODE_OUTPUT_STANDARD);
    platform_gpio_write(leds[ii], !LEDS_ACTIVE_STATE);
  }

  return err_code;
}

// TODO: send back error if type is not actuator configuration.
ruuvi_endpoint_status_t led_handler(ruuvi_standard_message_t* const message)
{
  if(NULL == message)                        { return ENDPOINT_HANDLER_NULL; }
  if(LED != message->destination_endpoint)   { return ENDPOINT_INVALID; }
  if(ACTUATOR_CONFIGRATION != message->type) { return ENDPOINT_INVALID; }

  ruuvi_actuator_configuration_t* config = (void*)message->payload;
  bool success = true;
  uint8_t leds[] = LEDS_LIST;
  
  if(LEDS_NUMBER > config->channel)
  {
      if(config->duty_cycle) { platform_gpio_write(leds[config->channel], LEDS_ACTIVE_STATE); }
      else                   { platform_gpio_write(leds[config->channel], !LEDS_ACTIVE_STATE); }
  }
  else
  {
      // Signal error by writing invalid to channel
      config->channel = ENDPOINT_INVALID;
      success = false;
  }

  // If message was understood, switch destination and source, set type as acknowledgement
  // and set payload to success
  uint8_t source = message->source_endpoint;
  message->source_endpoint = message->destination_endpoint;
  message->destination_endpoint = source;
  message->type = ACKNOWLEDGEMENT;
  if (success) { memset(message->payload, 0 , sizeof(message->payload)); }
  return ENDPOINT_SUCCESS;
}