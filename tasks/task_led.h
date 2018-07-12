#ifndef TASK_LEDS_H
#define TASK_LEDS_H
#include "ruuvi_error.h"

ruuvi_status_t task_led_init(void);
ruuvi_endpoint_status_t led_handler(ruuvi_standard_message_t* const message);

#endif