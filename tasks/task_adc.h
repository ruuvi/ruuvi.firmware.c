#ifndef TASK_ADC_H
#define TASK_ADC_H
#include "ruuvi_error.h"

ruuvi_status_t task_adc_init(void);

ruuvi_status_t task_adc_sample_asynchronous_battery(void);

#endif