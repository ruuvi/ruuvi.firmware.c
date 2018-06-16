#ifndef TASK_BLUETOOTH_H
#define TASK_BLUETOOTH_H
#include "ruuvi_error.h"
#include <stddef.h>

ruuvi_status_t task_bluetooth_init(void);
ruuvi_status_t task_bluetooth_advertise(uint8_t* data, size_t data_length);
ruuvi_status_t task_bluetooth_send_asynchronous(uint8_t* data, size_t data_length);
ruuvi_status_t task_bluetooth_process(void);

#endif