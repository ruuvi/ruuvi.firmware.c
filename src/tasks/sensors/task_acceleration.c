#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_acceleration.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_lis2dh12.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_yield.h"

#include "task_acceleration.h"
#include "task_gpio.h"
#include "task_communication.h"
#include "task_sensor.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define TASK_ACCELERATION_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#define LOG(x) ruuvi_interface_log(TASK_ACCELERATION_LOG_LEVEL, x);
#define LOGD(x) ruuvi_interface_log(RUUVI_INTERFACE_LOG_DEBUG, x);

static ruuvi_driver_sensor_t acceleration_sensor = {0};
static uint8_t m_nbr_movements;

static void task_acceleration_on_activity (ruuvi_interface_gpio_evt_t event)
{
    m_nbr_movements++;
}

ruuvi_driver_status_t task_acceleration_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
    ruuvi_driver_sensor_configuration_t config;
    config.samplerate    = APPLICATION_ACCELEROMETER_SAMPLERATE;
    config.resolution    = APPLICATION_ACCELEROMETER_RESOLUTION;
    config.scale         = APPLICATION_ACCELEROMETER_SCALE;
    config.dsp_function  = APPLICATION_ACCELEROMETER_DSPFUNC;
    config.dsp_parameter = APPLICATION_ACCELEROMETER_DSPPARAM;
    config.mode          = APPLICATION_ACCELEROMETER_MODE;
    uint8_t handle = 0;
    m_nbr_movements = 0;

    if (!task_gpio_is_init())
    {
        task_gpio_init();
    }

#if RUUVI_BOARD_ACCELEROMETER_LIS2DH12_PRESENT
    err_code = RUUVI_DRIVER_SUCCESS;
    // Only SPI supported for now
    bus = RUUVI_DRIVER_BUS_SPI;
    handle = RUUVI_BOARD_SPI_SS_ACCELEROMETER_PIN;
    err_code |= ruuvi_interface_lis2dh12_init (&acceleration_sensor, bus, handle);
    RUUVI_DRIVER_ERROR_CHECK (err_code, ~RUUVI_DRIVER_ERROR_FATAL);

    if (RUUVI_DRIVER_SUCCESS == err_code)
    {
        ruuvi_interface_gpio_id_t pin = {.pin = RUUVI_BOARD_INT_ACC2_PIN};
        err_code |= task_sensor_configure (&acceleration_sensor, &config, "g");
        float ths = APPLICATION_ACCELEROMETER_ACTIVITY_THRESHOLD;
        err_code |= ruuvi_interface_lis2dh12_activity_interrupt_use (true, &ths);
        err_code |= ruuvi_interface_gpio_interrupt_enable (pin, RUUVI_INTERFACE_GPIO_SLOPE_LOTOHI,
                    RUUVI_INTERFACE_GPIO_MODE_INPUT_NOPULL, task_acceleration_on_activity);
        return err_code;
    }

#endif
    // Return error if usable acceleration sensor was not found.
    return RUUVI_DRIVER_ERROR_NOT_FOUND;
}


ruuvi_driver_status_t task_acceleration_data_get (ruuvi_driver_sensor_data_t *
        const data)
{
    if (NULL == data)
    {
        return RUUVI_DRIVER_ERROR_NULL;
    }

    if (NULL == acceleration_sensor.data_get)
    {
        return RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    ruuvi_driver_status_t err_code = acceleration_sensor.data_get (data);
    return err_code;
}

ruuvi_driver_status_t task_acceleration_on_button (void)
{
    // No implementation needed
    return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_acceleration_movement_count_get (uint8_t * const count)
{
    *count = m_nbr_movements;
    return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_acceleration_fifo_use (const bool enable)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (true == enable)
    {
        err_code |= ruuvi_interface_lis2dh12_fifo_use (true);
        err_code |= ruuvi_interface_lis2dh12_fifo_interrupt_use (true);
    }

    if (false == enable)
    {
        err_code |= ruuvi_interface_lis2dh12_fifo_use (false);
        err_code |= ruuvi_interface_lis2dh12_fifo_interrupt_use (false);
    }

    return err_code;
}