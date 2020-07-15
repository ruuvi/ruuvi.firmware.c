#include "app_config.h"
#include "app_sensor.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_i2c.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_lis2dh12.h"
#include "ruuvi_interface_adc_ntc.h"
#include "ruuvi_interface_adc_photo.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_shtcx.h"
#include "ruuvi_interface_spi.h"
#include "ruuvi_task_adc.h"
#include "ruuvi_task_sensor.h"

/**
 * @addtogroup app_sensor
 */
/** @{ */
/**
 * @file app_sensor.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-04-16
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Initialize, configure and use sensors on board.
 *
 * Typical usage:
 *
 * @code{.c}
 * TODO
 * @endcode
 */
#define APP_SENSOR_HANDLE_UNUSED (0xFFU) //!< Mark sensor unavailable with this handle.

#ifndef CEEDLING
static
#endif
rt_sensor_ctx_t * m_sensors[SENSOR_COUNT]; //!< Sensor APIs.
uint64_t vdd_update_time = 0;

#if APP_SENSOR_BME280_ENABLED
static rt_sensor_ctx_t bme280 =
{
    .sensor = {0},
    .init = &ri_bme280_init,
    .configuration =
    {
        .dsp_function  = APP_SENSOR_BME280_DSP_FUNC,
        .dsp_parameter = APP_SENSOR_BME280_DSP_PARAM,
        .mode          = APP_SENSOR_BME280_MODE,
        .resolution    = APP_SENSOR_BME280_RESOLUTION,
        .samplerate    = APP_SENSOR_BME280_SAMPLERATE,
        .scale         = APP_SENSOR_BME280_SCALE
    },
    .nvm_file = APP_FLASH_SENSOR_FILE,
    .nvm_record = APP_FLASH_SENSOR_BME280_RECORD,
#if RB_ENVIRONMENTAL_BME280_SPI_USE
    .bus = RD_BUS_SPI,
    .handle = RB_SPI_SS_ENVIRONMENTAL_PIN,
#elif RB_ENVIRONMENTAL_BME280_I2C_USE
    .bus = RD_BUS_I2C,
    .handle = RB_BME280_I2C_ADDRESS,
#else
#   error "No bus defined for BME280"
#endif
    .pwr_pin = RI_GPIO_ID_UNUSED,
    .pwr_on  = RI_GPIO_HIGH,
    .fifo_pin = RI_GPIO_ID_UNUSED,
    .level_pin = RI_GPIO_ID_UNUSED
};
#endif

#if APP_SENSOR_LIS2DH12_ENABLED
static rt_sensor_ctx_t lis2dh12 =
{
    .sensor = {0},
    .init = &ri_lis2dh12_init,
    .configuration =
    {
        .dsp_function  = APP_SENSOR_LIS2DH12_DSP_FUNC,
        .dsp_parameter = APP_SENSOR_LIS2DH12_DSP_PARAM,
        .mode          = APP_SENSOR_LIS2DH12_MODE,
        .resolution    = APP_SENSOR_LIS2DH12_RESOLUTION,
        .samplerate    = APP_SENSOR_LIS2DH12_SAMPLERATE,
        .scale         = APP_SENSOR_LIS2DH12_SCALE
    },
    .nvm_file = APP_FLASH_SENSOR_FILE,
    .nvm_record = APP_FLASH_SENSOR_LIS2DH12_RECORD,
    .bus = RD_BUS_SPI,
    .handle = RB_SPI_SS_ACCELEROMETER_PIN,
    .pwr_pin = RI_GPIO_ID_UNUSED,
    .pwr_on  = RI_GPIO_HIGH,
    .fifo_pin = RB_INT_ACC2_PIN,
    .level_pin = RB_INT_ACC1_PIN
};
#endif

#if APP_SENSOR_LIS2DW12_ENABLED
static rt_sensor_ctx_t lis2dw12 =
{
    .sensor = {0},
    .init = ri_lis2dw12_init,
    .configuration = {0},
    .nvm_file = APPLICATION_FLASH_SENSOR_FILE,
    .nvm_record = APPLICATION_FLASH_SENSOR_LIS2DW12_RECORD,
    .bus = RD_BUS_SPI,
    .handle = RB_SPI_SS_ACCELEROMETER_PIN,
    .pwr_pin = RI_GPIO_ID_UNUSED,
    .pwr_on  = RI_GPIO_HIGH,
    .fifo_pin = RB_INT_ACC1_PIN,
    .level_pin = RB_INT_ACC2_PIN
};
#endif

#if APP_SENSOR_SHTCX_ENABLED
static rt_sensor_ctx_t shtcx =
{
    .sensor = {0},
    .init = &ri_shtcx_init,
    .configuration =
    {
        .dsp_function  = APP_SENSOR_SHTCX_DSP_FUNC,
        .dsp_parameter = APP_SENSOR_SHTCX_DSP_PARAM,
        .mode          = APP_SENSOR_SHTCX_MODE,
        .resolution    = APP_SENSOR_SHTCX_RESOLUTION,
        .samplerate    = APP_SENSOR_SHTCX_SAMPLERATE,
        .scale         = APP_SENSOR_SHTCX_SCALE
    },
    .nvm_file = APP_FLASH_SENSOR_FILE,
    .nvm_record = APP_FLASH_SENSOR_SHTCX_RECORD,
    .bus = RD_BUS_I2C,
    .handle = RB_SHTCX_I2C_ADDRESS,
    .pwr_pin = RI_GPIO_ID_UNUSED,
    .pwr_on  = RI_GPIO_HIGH,
    .fifo_pin = RI_GPIO_ID_UNUSED,
    .level_pin = RI_GPIO_ID_UNUSED
};
#endif

#if APP_SENSOR_PHOTO_ENABLED
static rt_sensor_ctx_t photo =
{
    .sensor = {0},
    .init = &ri_adc_photo_init,
    .configuration = {0},
    .nvm_file = APP_FLASH_SENSOR_FILE,
    .nvm_record = APP_FLASH_SENSOR_PHOTO_RECORD,
    .bus = RD_BUS_NONE,
    .handle = RB_PHOTO_ADC,
    .pwr_pin = RB_PHOTO_PWR_PIN,
    .pwr_on = RB_PHOTO_ACTIVE,
    .fifo_pin = RI_GPIO_ID_UNUSED,
    .level_pin = RI_GPIO_ID_UNUSED
};
#endif

#if APP_SENSOR_NTC_ENABLED
static rt_sensor_ctx_t ntc =
{
    .sensor = {0},
    .init = &ri_adc_ntc_init,
    .configuration = {0},
    .nvm_file = APP_FLASH_SENSOR_FILE,
    .nvm_record = APP_FLASH_SENSOR_NTC_RECORD,
    .bus = RD_BUS_NONE,
    .handle = RB_NTC_ADC,
    .pwr_pin = RB_NTC_PWR_PIN,
    .pwr_on = RB_NTC_ACTIVE,
    .fifo_pin = RI_GPIO_ID_UNUSED,
    .level_pin = RI_GPIO_ID_UNUSED
};
#endif

/** @brief Initialize sensor pointer array */
#ifndef CEEDLING
static
#endif
void m_sensors_init (void)
{
#if APP_SENSOR_TMP117_ENABLED
    m_sensors[TMP117_INDEX] = tmp117;
#endif
#if APP_SENSOR_SHTCX_ENABLED
    m_sensors[SHTCX_INDEX] = &shtcx;
#endif
#if APP_SENSOR_BME280_ENABLED
    m_sensors[BME280_INDEX] = &bme280;
#endif
#if APP_SENSOR_NTC_ENABLED
    m_sensors[NTC_INDEX] = &ntc;
#endif
#if APP_SENSOR_PHOTO_ENABLED
    m_sensors[PHOTO_INDEX] = &photo;
#endif
#if APP_SENSOR_MCU_ENABLED
    m_sensors[ENV_MCU_INDEX] = env_mcu;
#endif
#if APP_SENSOR_LIS2DH12_ENABLED
    m_sensors[ LIS2DH12_INDEX] = &lis2dh12;
#endif
#if APP_SENSOR_LIS2DW12_ENABLED
    m_sensors[LIS2DW12_INDEX] = lis2dw12;
#endif
}

// Measure battery voltage after radio event
#ifndef CEEDLING
static
#endif
void on_radio (const ri_radio_activity_evt_t evt)
{
    rd_status_t err_code = RD_SUCCESS;

    if (vdd_update_time < ri_rtc_millis())
    {
        if (RI_RADIO_BEFORE == evt)
        {
            rd_sensor_configuration_t configuration =
            {
                .dsp_function  = RD_SENSOR_CFG_DEFAULT,
                .dsp_parameter = RD_SENSOR_CFG_DEFAULT,
                .mode          = RD_SENSOR_CFG_SINGLE,
                .resolution    = RD_SENSOR_CFG_DEFAULT,
                .samplerate    = RD_SENSOR_CFG_DEFAULT,
                .scale         = RD_SENSOR_CFG_DEFAULT
            };
            err_code |= rt_adc_vdd_prepare (&configuration);
            RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
        }
        else
        {
            if (true == rt_adc_is_init())
            {
                vdd_update_time = ri_rtc_millis();
                vdd_update_time += APP_BATTERY_SAMPLE_MS;
                err_code |= rt_adc_vdd_sample();
                RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
            }
        }
    }
}

static ri_i2c_frequency_t rb_to_ri_i2c_freq (unsigned int rb_freq)
{
    ri_i2c_frequency_t freq = RI_I2C_FREQUENCY_100k;

    switch (rb_freq)
    {
        case RB_I2C_FREQUENCY_400k:
            freq = RI_I2C_FREQUENCY_400k;
            break;

        case RB_I2C_FREQUENCY_250k:
            freq = RI_I2C_FREQUENCY_250k;
            break;

        case RB_I2C_FREQUENCY_100k:

        // Intentional fall-through.
        default:
            freq = RI_I2C_FREQUENCY_100k;
            break;
    }

    return freq;
}

static ri_spi_frequency_t rb_to_ri_spi_freq (unsigned int rb_freq)
{
    ri_spi_frequency_t freq = RI_SPI_FREQUENCY_1M;

    switch (rb_freq)
    {
        case RB_SPI_FREQUENCY_8M:
            freq = RI_SPI_FREQUENCY_8M;
            break;

        case RB_SPI_FREQUENCY_4M:
            freq = RI_SPI_FREQUENCY_4M;
            break;

        case RB_SPI_FREQUENCY_2M:
            freq = RI_SPI_FREQUENCY_2M;
            break;

        case RB_SPI_FREQUENCY_1M:

        // Intentional fall-through.
        default:
            freq = RI_SPI_FREQUENCY_1M;
            break;
    }

    return freq;
}

static rd_status_t app_sensor_buses_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_gpio_id_t ss_pins[RB_SPI_SS_NUMBER] = RB_SPI_SS_LIST;
    ri_spi_init_config_t spi_config =
    {
        .mosi = RB_SPI_MOSI_PIN,
        .miso = RB_SPI_MISO_PIN,
        .sclk = RB_SPI_SCLK_PIN,
        .ss_pins = ss_pins,
        .ss_pins_number = sizeof (ss_pins) / sizeof (ri_gpio_id_t),
        // Assume mode 0 always.
        .mode = RI_SPI_MODE_0,
        .frequency = rb_to_ri_spi_freq (RB_SPI_FREQ)
    };
    err_code |= ri_spi_init (&spi_config);
    ri_i2c_init_config_t i2c_config =
    {
        .sda = RB_I2C_SDA_PIN,
        .scl = RB_I2C_SCL_PIN,
        .frequency = rb_to_ri_i2c_freq (RB_I2C_FREQ)
    };
    err_code |= ri_i2c_init (&i2c_config);
    return err_code;
}

static rd_status_t app_sensor_buses_uninit (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_spi_uninit();
    err_code |= ri_i2c_uninit ();
    return err_code;
}

static void app_sensor_rtc_init (void)
{
    // Returns invalid state if already init, not a problem here.
    (void) ri_rtc_init();
    rd_sensor_timestamp_function_set (&ri_rtc_millis);
}


static void app_sensor_rtc_uninit (void)
{
    rd_sensor_timestamp_function_set (NULL);
    (void) ri_rtc_uninit();
}

rd_status_t app_sensor_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    m_sensors_init();
    app_sensor_buses_init();
    app_sensor_rtc_init();

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        rd_status_t init_code = RD_SUCCESS;
        size_t retries = 0;

        // Enable power to sensor
        if (m_sensors[ii]->pwr_pin != RI_GPIO_ID_UNUSED)
        {
            (void) ri_gpio_configure (m_sensors[ii]->pwr_pin,
                                      RI_GPIO_MODE_OUTPUT_HIGHDRIVE);
            (void) ri_gpio_write (m_sensors[ii]->pwr_pin, m_sensors[ii]->pwr_on);
        }

        // Some sensors, such as accelerometer may fail on user moving the board. Retry.
        do
        {
            init_code = rt_sensor_initialize (m_sensors[ii]);
        } while ( (APP_SENSOR_SELFTEST_RETRIES > retries++)

                  && (RD_ERROR_SELFTEST == init_code));

        if (RD_SUCCESS == init_code)
        {
            // Check for a configuration in flash.
            init_code = rt_sensor_load (m_sensors[ii]);

            // Configuration found, use it.
            if (RD_SUCCESS == init_code)
            {
                init_code = rt_sensor_configure (m_sensors[ii]);
            }
            // Configuration not found, use defaults, store to flash.
            else
            {
                init_code = rt_sensor_configure (m_sensors[ii]);
                rt_sensor_store (m_sensors[ii]);
            }
        }
        else if (RD_ERROR_SELFTEST == init_code)
        {
            err_code |= RD_ERROR_SELFTEST;
        }
        // Mark unavailable sensor handles as unused.
        else
        {
            m_sensors[ii]->handle = APP_SENSOR_HANDLE_UNUSED;
        }
    }

    // Synchronize battery measurement to radio activity.
    ri_radio_activity_callback_set (on_radio);
    return err_code;
}

rd_status_t app_sensor_uninit (void)
{
    rd_status_t err_code = RD_SUCCESS;

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        if ( (NULL != m_sensors[ii]) && rd_sensor_is_init (& (m_sensors[ii]->sensor)))
        {
            m_sensors[ii]->sensor.uninit (&m_sensors[ii]->sensor, m_sensors[ii]->bus,
                                          m_sensors[ii]->handle);

            // Disable power to sensor
            if (m_sensors[ii]->pwr_pin != RI_GPIO_ID_UNUSED)
            {
                (void) ri_gpio_write (m_sensors[ii]->pwr_pin, !m_sensors[ii]->pwr_on);
                (void) ri_gpio_configure (m_sensors[ii]->pwr_pin, RI_GPIO_MODE_HIGH_Z);
            }
        }
    }

    err_code |= app_sensor_buses_uninit();
    app_sensor_rtc_uninit();
    ri_radio_activity_callback_set (NULL);
    return err_code;
}

rd_sensor_data_fields_t app_sensor_available_data (void)
{
    rd_sensor_data_fields_t available = {0};

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        if ( (NULL != m_sensors[ii])
                && rd_sensor_is_init (& (m_sensors[ii]->sensor)))
        {
            available.bitfield |= m_sensors[ii]->sensor.provides.bitfield;
        }
    }

    return available;
}

rd_status_t app_sensor_get (rd_sensor_data_t * const data)
{
    rd_status_t err_code = RD_SUCCESS;

    for (size_t ii = 0; ii < SENSOR_COUNT; ii++)
    {
        if ( (NULL != m_sensors[ii])
                && rd_sensor_is_init (& (m_sensors[ii]->sensor)))
        {
            err_code |= m_sensors[ii]->sensor.data_get (data);
        }
    }

    return err_code;
}

#ifdef RUUVI_RUN_TESTS
void app_sensor_ctx_get (rt_sensor_ctx_t *** p_sensors, size_t * num_sensors)
{
    *p_sensors = m_sensors;
    *num_sensors = SENSOR_COUNT;
}
#endif

/** @} */
