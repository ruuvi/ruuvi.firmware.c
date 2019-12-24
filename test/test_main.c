#include "unity.h"

#include "main.h"

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_endpoints.h"
#include "mock_ruuvi_interface_communication_radio.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_rtc.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_watchdog.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_task_acceleration.h"
#include "mock_task_adc.h"
#include "mock_task_advertisement.h"
#include "mock_task_button.h"
#include "mock_task_communication.h"
#include "mock_task_environmental.h"
#include "mock_task_flash.h"
#include "mock_task_gatt.h"
#include "mock_task_gpio.h"
#include "mock_task_led.h"
#include "mock_task_i2c.h"
#include "mock_task_nfc.h"
#include "mock_task_power.h"
#include "mock_task_rtc.h"
#include "mock_task_scheduler.h"
#include "mock_task_sensor.h"
#include "mock_task_spi.h"
#include "mock_task_timer.h"
#include "mock_test_sensor.h"
#include "mock_test_acceleration.h"
#include "mock_test_adc.h"
#include "mock_test_environmental.h"
#include "mock_test_library.h"

void setUp (void)
{
}

void tearDown (void)
{
}

void test_main_NeedToImplement (void)
{
    TEST_IGNORE_MESSAGE ("Need to Implement main");
}
