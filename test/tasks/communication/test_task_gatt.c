#include "unity.h"

#include "application_config.h"

#include "task_gatt.h"

#include "task_adc.h"
#include "task_advertisement.h"
#include "task_communication.h"


#include "ruuvi_driver_error.h"

#include "mock_ruuvi_interface_atomic.h"
#include "mock_ruuvi_interface_adc_mcu.h"
#include "mock_ruuvi_interface_communication_ble4_advertising.h"
#include "mock_ruuvi_interface_communication_ble4_gatt.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_timer.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_ruuvi_interface_watchdog.h"
#include "mock_ruuvi_library_ringbuffer.h"
#include "mock_task_environmental.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_task_gatt_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement task_gatt");
}
