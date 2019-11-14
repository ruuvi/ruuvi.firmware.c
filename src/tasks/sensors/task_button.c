#include "ruuvi_driver_error.h"
#include "ruuvi_boards.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_interface_log.h"
#include "task_acceleration.h"
#include "task_adc.h"
#include "task_advertisement.h"
#include "task_button.h"
#include "task_environmental.h"
#include "task_led.h"
#include "task_rtc.h"
#include <stddef.h>
#if APPLICATION_BUTTON_ENABLED
static task_button_fp_t button_callback = NULL;

// Wrapper to gpio interrupt function
static void on_button(ruuvi_interface_gpio_evt_t event)
{
  if(NULL != button_callback) { button_callback(); }

  ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO, "Button\r\n");
}

ruuvi_driver_status_t task_button_init(ruuvi_interface_gpio_slope_t slope,
                                       task_button_fp_t action)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  button_callback = action;
  ruuvi_interface_gpio_id_t pin = {.pin = RUUVI_BOARD_BUTTON_1};
  err_code |= ruuvi_interface_gpio_interrupt_enable(pin, slope,
              RUUVI_INTERFACE_GPIO_MODE_INPUT_PULLUP, on_button);
  return err_code;
}

ruuvi_driver_status_t task_button_on_press(void)
{
  static uint64_t last_press = 0;
  // returns UINT64_MAX if RTC is not running.
  uint64_t now = task_rtc_millis();
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

  // Debounce button
  if((now - last_press) > RUUVI_BOARD_BUTTON_DEBOUNCE_PERIOD_MS)
  {
    // Do your button action here
    // p_event_data points to action data,
    // event size is size of action data at most equal to APPLICATION_TASK_DATA_MAX_SIZE
    // handler is function pointer which will handle this event
    //ruuvi_interface_scheduler_event_put (void const *p_event_data, uint16_t event_size, ruuvi_scheduler_event_handler_t handler);
  }

  // store time of press for debouncing if possible
  if(RUUVI_DRIVER_UINT64_INVALID != now) { last_press = now; }

  return err_code;
}
#else
ruuvi_driver_status_t task_button_init(ruuvi_interface_gpio_slope_t slope,
                                       task_button_fp_t action)
{
  return RUUVI_DRIVER_SUCCESS;
}
#endif