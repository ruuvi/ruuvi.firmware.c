
/**
 * @addtogroup adc_tasks
 */
/*@{*/
/**
 * @file task_adc.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-11-28
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "application_config.h"
#include "task_adc.h"

#include <stdbool.h>
#include <string.h>

#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_adc_mcu.h"
#include "ruuvi_interface_atomic.h"

static ruuvi_interface_atomic_t m_is_init;
static bool m_is_configured;
static bool m_vdd_prepared;
static bool m_vdd_sampled;
static float m_vdd;
static ruuvi_driver_sensor_t m_adc; //!< ADC control instance

ruuvi_driver_status_t task_adc_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (!ruuvi_interface_atomic_flag (&m_is_init, true))
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    return err_code;
}

ruuvi_driver_status_t task_adc_uninit (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    m_is_configured = false;
    m_vdd_prepared = false;
    m_vdd_sampled = false;
    err_code |= ruuvi_interface_adc_mcu_uninit (&m_adc, RUUVI_DRIVER_BUS_NONE, 0);

    if (!ruuvi_interface_atomic_flag (&m_is_init, false))
    {
        err_code |= RUUVI_DRIVER_ERROR_FATAL;
    }

    return err_code;
}

/**
 * @brief Check if ADC is initialized.
 *
 * @retval true if ADC is initialized.
 * @retval false if ADC is not initialized.
 */
inline bool task_adc_is_init (void)
{
    return (0 != m_is_init);
}

ruuvi_driver_status_t task_adc_configure_se (ruuvi_driver_sensor_configuration_t * const
        config, const uint8_t handle, const task_adc_mode_t mode)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (!task_adc_is_init() || m_is_configured)
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        // TODO @ojousima: Support ratiometric
        if (ABSOLUTE == mode)
        {
            err_code |= ruuvi_interface_adc_mcu_init (&m_adc, RUUVI_DRIVER_BUS_NONE, handle);
            err_code |= m_adc.configuration_set (&m_adc, config);
        }
        else
        {
            err_code |= RUUVI_DRIVER_ERROR_NOT_IMPLEMENTED;
        }
    }

    if (RUUVI_DRIVER_SUCCESS == err_code)
    {
        m_is_configured = true;
    }

    return err_code;
}

ruuvi_driver_status_t task_adc_sample (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (!task_adc_is_init() || !m_is_configured || (NULL == m_adc.mode_set))
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
        err_code |= m_adc.mode_set (&mode);
    }

    return err_code;
}

ruuvi_driver_status_t task_adc_voltage_get (ruuvi_driver_sensor_data_t * const data)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (!task_adc_is_init() || !m_is_configured)
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        err_code |= m_adc.data_get (data);
    }

    return err_code;
}

ruuvi_driver_status_t task_adc_ratio_get (ruuvi_driver_sensor_data_t * const data)
{
    return RUUVI_DRIVER_ERROR_NOT_IMPLEMENTED;
}

ruuvi_driver_status_t task_adc_vdd_prepare (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_driver_sensor_configuration_t vdd_adc_configuration =
    {
        .dsp_function  = APPLICATION_ADC_DSPFUNC,
        .dsp_parameter = APPLICATION_ADC_DSPPARAM,
        .mode          = APPLICATION_ADC_MODE,
        .resolution    = APPLICATION_ADC_RESOLUTION,
        .samplerate    = APPLICATION_ADC_SAMPLERATE,
        .scale         = APPLICATION_ADC_SCALE
    };
    err_code |= task_adc_init();
    err_code |= task_adc_configure_se (&vdd_adc_configuration, RUUVI_INTERFACE_ADC_AINVDD,
                                       ABSOLUTE);
    m_vdd_prepared = (RUUVI_DRIVER_SUCCESS == err_code);
    return (RUUVI_DRIVER_SUCCESS == err_code) ? RUUVI_DRIVER_SUCCESS :
           RUUVI_DRIVER_ERROR_BUSY;
}

ruuvi_driver_status_t task_adc_vdd_sample (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (!m_vdd_prepared)
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }
    else
    {
        ruuvi_driver_sensor_data_t battery;
        memset (&battery, 0, sizeof (ruuvi_driver_sensor_data_t));
        float battery_values;
        battery.data = &battery_values;
        battery.fields.datas.voltage_v = 1;
        err_code |= task_adc_sample();
        err_code |= task_adc_voltage_get (&battery);
        m_vdd = ruuvi_driver_sensor_data_parse (&battery, battery.fields);
        err_code |= task_adc_uninit();
        m_vdd_prepared = false;
        m_vdd_sampled = true;
    }

    return err_code;
}

ruuvi_driver_status_t task_adc_vdd_get (float * const battery)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;

    if (true == m_vdd_sampled)
    {
        *battery = m_vdd;
    }
    else
    {
        err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    return err_code;
}

/*@}*/