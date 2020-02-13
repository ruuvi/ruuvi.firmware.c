#ifndef APP_LED_H
#define APP_LED_H
#include "ruuvi_driver_error.h"

/** @brief time each led is on at boot */
#define APP_LED_INIT_DELAY_MS (500U)

rd_status_t app_led_init (void);

#endif // APP_LED_H
