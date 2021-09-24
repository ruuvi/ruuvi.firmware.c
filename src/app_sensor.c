#include "app_config.h"
#include "app_sensor.h"
#include "app_comms.h"
#include "app_heartbeat.h"
#include "app_log.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_adc_ntc.h"
#include "ruuvi_interface_adc_photo.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_communication_ble_gatt.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_dps310.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_gpio_interrupt.h"
#include "ruuvi_interface_i2c.h"
#include "ruuvi_interface_lis2dh12.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_shtcx.h"
#include "ruuvi_interface_spi.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_adc.h"
#include "ruuvi_task_sensor.h"

#include <stdio.h>
#include <string.h>

#define POWERUP_DELAY_MS (10U)

static inline void LOG (const char * const msg)
{
    ri_log (RI_LOG_LEVEL_INFO, msg);
}

static inline void LOGD (const char * const msg)
{
    ri_log (RI_LOG_LEVEL_DEBUG, msg);
}
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

#ifndef CEEDLING
static
#endif
rt_sensor_ctx_t * m_sensors[SENSOR_COUNT]; //!< Sensor APIs.
static uint64_t vdd_update_time;              //!< timestamp of VDD update.
static uint32_t
m_event_counter;              //!< Number of events registered in app_sensor.

/**
 * @brief Sensor operation, such as read or configure.
 *
 * These are used when outside central commands sensor to e.g. configure a sensor
 * or read log. Operations are targeted on specific data types, such as
 * temperature or acceleration. The operation is execcuted on first
 * provider of given data if applicable.
 *
 * @param[in] reply_fp A function to which send operation acknowledgement.
 * @param[in] fields Affected fields.
 * @param[in] raw_message Original message triggering operation.
 */
typedef rd_status_t (*sensor_op) (const ri_comm_xfer_fp_t reply_fp,
                                  const rd_sensor_data_fields_t fields,
                                  const uint8_t * const raw_message);

#if APP_SENSOR_BME280_ENABLED
static rt_sensor_ctx_t bme280 = APP_SENSOR_BME280_DEFAULT_CFG;
#endif

#if APP_SENSOR_DPS310_ENABLED
static rt_sensor_ctx_t dps310 = APP_SENSOR_DPS310_DEFAULT_CFG;
#endif

#if APP_SENSOR_LIS2DH12_ENABLED
static rt_sensor_ctx_t lis2dh12 = APP_SENSOR_LIS2DH12_DEFAULT_CFG;
#endif

#if APP_SENSOR_LIS2DW12_ENABLED
static rt_sensor_ctx_t lis2dw12 = APP_SENSOR_LIS2DW2_DEFAULT_CFG;
#endif

#if APP_SENSOR_SHTCX_ENABLED
static rt_sensor_ctx_t shtcx = APP_SENSOR_SHTCX_DEFAULT_CFG;
#endif

#if APP_SENSOR_TMP117_ENABLED
static rt_sensor_ctx_t tmp117 = APP_SENSOR_TMP117_DEFAULT_CFG;
#endif

#if APP_SENSOR_PHOTO_ENABLED
static rt_sensor_ctx_t photo = APP_SENSOR_PHOTO_DEFAULT_CFG;
#endif

#if APP_SENSOR_NTC_ENABLED
static rt_sensor_ctx_t ntc = APP_SENSOR_NTC_DEFAULT_CFG;
#endif

#if APP_SENSOR_ENVIRONMENTAL_MCU_ENABLED
static rt_sensor_ctx_t env_mcu = APP_SENSOR_ENVIRONMENTAL_MCU_DEFAULT_CFG;
#endif

/** @brief Initialize sensor pointer array */
#ifndef CEEDLING
static
#endif
void
m_sensors_init (void)
{
#if APP_SENSOR_TMP117_ENABLED
    m_sensors[TMP117_INDEX] = &tmp117;
#endif
#if APP_SENSOR_SHTCX_ENABLED
    m_sensors[SHTCX_INDEX] = &shtcx;
#endif
#if APP_SENSOR_DPS310_ENABLED
    m_sensors[DPS310_INDEX] = &dps310;
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
#if APP_SENSOR_ENVIRONMENTAL_MCU_ENABLED
    m_sensors[ENV_MCU_INDEX] = &env_mcu;
#endif
#if APP_SENSOR_LIS2DH12_ENABLED
    m_sensors[LIS2DH12_INDEX] = &lis2dh12;
#endif
#if APP_SENSOR_LIS2DW12_ENABLED
    m_sensors[LIS2DW12_INDEX] = lis2dw12;
#endif
}

void app_sensor_vdd_measure_isr (const ri_radio_activity_evt_t evt)
{
    rd_status_t err_code = RD_SUCCESS;

    if (vdd_update_time < ri_rtc_millis())
    {
        if (RI_RADIO_BEFORE == evt)
        {
            rd_sensor_configuration_t configuration =
            {
                .dsp_function = RD_SENSOR_CFG_DEFAULT,
                .dsp_parameter = RD_SENSOR_CFG_DEFAULT,
                .mode = RD_SENSOR_CFG_SINGLE,
                .resolution = RD_SENSOR_CFG_DEFAULT,
                .samplerate = RD_SENSOR_CFG_DEFAULT,
                .scale = RD_SENSOR_CFG_DEFAULT
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

#ifndef CEEDLING
static
#endif
void
on_accelerometer_isr (const ri_gpio_evt_t event)
{
    if (RI_GPIO_SLOPE_LOTOHI == event.slope)
    {
        LOG ("Movement \r\n");
        app_sensor_event_increment();
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
    ri_i2c_init_config_t i2c_config =
    {
        .sda = RB_I2C_SDA_PIN,
        .scl = RB_I2C_SCL_PIN,
        .bus_pwr = RB_I2C_BUS_POWER_PIN,
        .frequency = rb_to_ri_i2c_freq (RB_I2C_FREQ)
    };

    if ( (!ri_gpio_is_init()) || (!ri_gpio_interrupt_is_init()))
    {
        err_code |= RD_ERROR_INVALID_STATE;
    }
    else
    {
        err_code |= ri_spi_init (&spi_config);
        err_code |= ri_i2c_init (&i2c_config);
        err_code |= ri_gpio_configure (RB_I2C_SDA_PIN,
                                       RI_GPIO_MODE_SINK_PULLUP_HIGHDRIVE);
        err_code |= ri_gpio_configure (RB_I2C_SCL_PIN,
                                       RI_GPIO_MODE_SINK_PULLUP_HIGHDRIVE);
    }

    return err_code;
}

static rd_status_t app_sensor_buses_uninit (void)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= ri_spi_uninit();
    err_code |= ri_i2c_uninit();
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
    err_code |= app_sensor_buses_init();

    if (RD_SUCCESS == err_code)
    {
        app_sensor_rtc_init();
        // Wait for the power lines to settle after bus powerup.
        ri_delay_ms (POWERUP_DELAY_MS);

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
                ri_delay_ms (POWERUP_DELAY_MS);
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
    }

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
        if ( (NULL != m_sensors[ii]) && rd_sensor_is_init (& (m_sensors[ii]->sensor)))
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
        if ( (NULL != m_sensors[ii]) && rd_sensor_is_init (& (m_sensors[ii]->sensor)))
        {
            err_code |= m_sensors[ii]->sensor.data_get (data);
        }
    }

    return err_code;
}

rd_sensor_t * app_sensor_find_provider (const rd_sensor_data_fields_t data)
{
    rd_sensor_t * provider = NULL;

    for (size_t ii = 0; (ii < SENSOR_COUNT) && (NULL == provider); ii++)
    {
        if ( (NULL != m_sensors[ii]) && rd_sensor_is_init (& (m_sensors[ii]->sensor))
                && (! (~ (m_sensors[ii]->sensor.provides.bitfield) & data.bitfield)))
        {
            provider = & (m_sensors[ii]->sensor);
        }
    }

    return provider;
}

void app_sensor_event_increment (void)
{
    m_event_counter++;
}

uint32_t app_sensor_event_count_get (void)
{
    return m_event_counter;
}

rd_status_t app_sensor_acc_thr_set (float * const threshold_g)
{
    rd_status_t err_code = RD_SUCCESS;
    const rd_sensor_data_fields_t acceleration =
    {
        .datas.acceleration_x_g = 1,
        .datas.acceleration_y_g = 1,
        .datas.acceleration_z_g = 1
    };
    const rd_sensor_t * const provider = app_sensor_find_provider (acceleration);

    if (RI_GPIO_ID_UNUSED == RB_INT_LEVEL_PIN)
    {
        err_code |= RD_ERROR_NOT_SUPPORTED;
    }
    else if ( (NULL == provider) || (NULL == provider->level_interrupt_set))
    {
        err_code |= RD_ERROR_NOT_SUPPORTED;
    }
    else if (NULL == threshold_g)
    {
        ri_gpio_interrupt_disable (RB_INT_LEVEL_PIN);
        err_code |= provider->level_interrupt_set (false, threshold_g);
    }
    else if (0 > *threshold_g)
    {
        err_code |= RD_ERROR_NOT_IMPLEMENTED;
    }
    else
    {
        err_code |= ri_gpio_interrupt_enable (RB_INT_LEVEL_PIN,
                                              RI_GPIO_SLOPE_TOGGLE,
                                              RI_GPIO_MODE_INPUT_NOPULL,
                                              &on_accelerometer_isr);
        err_code |= provider->level_interrupt_set (true, threshold_g);
    }

    return err_code;
}

/**
 * @brief Determine which fields are affected by given endpoint.
 *
 * @param[in] type Ruuvi Endpoint type.
 * @return Ruuvi Driver fields corresponding to endpoint.
 */
static rd_sensor_data_fields_t re2rd_fields (const re_type_t type)
{
    rd_sensor_data_fields_t fields = {0};

    switch (type)
    {
        case RE_ACC_XYZ:
            fields.datas.acceleration_x_g = 1;
            fields.datas.acceleration_y_g = 1;
            fields.datas.acceleration_z_g = 1;
            break;

        case RE_ACC_X:
            fields.datas.acceleration_x_g = 1;
            break;

        case RE_ACC_Y:
            fields.datas.acceleration_y_g = 1;
            break;

        case RE_ACC_Z:
            fields.datas.acceleration_z_g = 1;
            break;

        case RE_GYR_XYZ:
            fields.datas.gyro_x_dps = 1;
            fields.datas.gyro_y_dps = 1;
            fields.datas.gyro_z_dps = 1;
            break;

        case RE_GYR_X:
            fields.datas.gyro_x_dps = 1;
            break;

        case RE_GYR_Y:
            fields.datas.gyro_y_dps = 1;
            break;

        case RE_GYR_Z:
            fields.datas.gyro_z_dps = 1;
            break;

        case RE_ENV_ALL:
            fields.datas.humidity_rh = 1;
            fields.datas.pressure_pa = 1;
            fields.datas.temperature_c = 1;
            break;

        case RE_ENV_HUMI:
            fields.datas.humidity_rh = 1;
            break;

        case RE_ENV_PRES:
            fields.datas.pressure_pa = 1;
            break;

        case RE_ENV_TEMP:
            fields.datas.temperature_c = 1;
            break;

        default:
            RD_ERROR_CHECK (RD_ERROR_NOT_IMPLEMENTED, ~RD_ERROR_FATAL);
            break;
    }

    return fields;
}

/**
 * @brief Convert Ruuvi Driver data type to Ruuvi endpoint header.
 *
 * @param[in] field Data field to convert, exactly one must be set.
 * @return Ruuvi Endpoint constant corresponding to field. 0 if field is invalid.
 */
static uint8_t rd2re_fields (const rd_sensor_data_bitfield_t fields)
{
    // Implementing field->header assigments as a look-up table would
    // rely on specific representation of bitfield at compile time.
    // Little-endian, big-endian, BE-8, BE-32 etc. Using if-else
    // here for robustness.
    uint8_t header_value = 0;

    if (fields.acceleration_x_g)
    {
        header_value = RE_ACC_X;
    }
    else if (fields.acceleration_y_g)
    {
        header_value = RE_ACC_Y;
    }
    else if (fields.acceleration_z_g)
    {
        header_value = RE_ACC_Z;
    }
    else if (fields.gyro_x_dps)
    {
        header_value = RE_GYR_X;
    }
    else if (fields.gyro_y_dps)
    {
        header_value = RE_GYR_Y;
    }
    else if (fields.gyro_z_dps)
    {
        header_value = RE_GYR_Z;
    }
    else if (fields.humidity_rh)
    {
        header_value = RE_ENV_HUMI;
    }
    else if (fields.pressure_pa)
    {
        header_value = RE_ENV_PRES;
    }
    else if (fields.temperature_c)
    {
        header_value = RE_ENV_TEMP;
    }
    else
    {
        // No action needed
    }

    return header_value;
}

/**
 * @brief encode log element into given buffer.
 *
 * @param[in] buffer Buffer to encode data into.
 * @param[in] timestamp_ms Float value to encode.
 * @param[in] data Float value to encode.
 * @param[in] type Type of data to encode. Only one type/message is implemented.
 *
 * @note This function will not set the destination endpoint in buffer.
 * @retval RD_SUCCESS Data was encoded successfully.
 * @retval RD_ERROR_NULL Buffer or data was NULL.
 * @retval RD_ERROR_INVALID_PARAM Type had no field set.
 * @retval RD_ERROR_NOT_IMPLEMENTED Type had > 1 field set or encoding type is not implemented.
 */
static rd_status_t app_sensor_encode_log (uint8_t * const buffer,
        const uint64_t timestamp_ms,
        const float data,
        const rd_sensor_data_bitfield_t type)
{
    rd_status_t err_code = RD_SUCCESS;
    const uint8_t source = rd2re_fields (type);

    if (0 != source)
    {
        re_log_write_header (buffer, source);
        re_log_write_timestamp (buffer, timestamp_ms);
        re_log_write_data (buffer, data, source);
    }
    else
    {
        err_code |= RD_ERROR_INVALID_PARAM;
    }

    return err_code;
}

/**
 * if valid data in sample
 * parse data type
 * parse data value
 * format msg
 * send msg
 */
static rd_status_t send_field (const ri_comm_xfer_fp_t reply_fp,
                               const uint8_t * const raw_message,
                               const rd_sensor_data_bitfield_t type,
                               const float value,
                               const int64_t real_time_ms)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_comm_message_t msg = {0};
    err_code |= app_sensor_encode_log (msg.data, real_time_ms, value,
                                       type);

    if (RD_SUCCESS == err_code)
    {
        msg.repeat_count = 1;
        msg.data_length = RE_STANDARD_MESSAGE_LENGTH;
        msg.data[RE_STANDARD_DESTINATION_INDEX] = raw_message[RE_STANDARD_SOURCE_INDEX];
        err_code |= app_comms_blocking_send (reply_fp, &msg);
    }

    return err_code;
}

/**
 * @brief Send data element.
 *
 * This function sends given data element to given reply function pointer.
 *
 * @param[in] reply_fp Function pointer to reply to.
 * @param[in] raw_message original query from remote.
 * @param[in] sample Data sample to send.
 * @param[in] time_offset_ms Offset between tag time and real time.
 * @retval RD_SUCCESS reply was sent successfully.
 * @retval error code from reply_fp in case of error.
 *
 * @note This function blocks until reply_fp returns something else than
 *       RD_ERROR_NO_MEM.
 */
static rd_status_t app_sensor_send_data (const ri_comm_xfer_fp_t reply_fp,
        const uint8_t * const raw_message,
        const rd_sensor_data_t * const sample,
        const int64_t time_offset_ms)
{
    rd_status_t err_code = RD_SUCCESS;
    const uint8_t fieldcount = rd_sensor_data_fieldcount (sample);
    int64_t real_time_ms = 0;

    if ( (0 - time_offset_ms) < (int64_t) sample->timestamp_ms)
    {
        real_time_ms = sample->timestamp_ms + time_offset_ms;
    }

    for (uint8_t ii = 0; ii < fieldcount; ii++)
    {
        if (rd_sensor_has_valid_data (sample, ii))
        {
            rd_sensor_data_bitfield_t type = rd_sensor_field_type (sample, ii);
            err_code |= send_field (reply_fp, raw_message,
                                    type, sample->data[ii], real_time_ms);
        }
    }

    return err_code;
}

/**
 * @brief Send no more sensor log data message.
 * TODO -refactor encoding to endpoints.
 * TODO -refactor into comms
 */
static rd_status_t app_sensor_send_eof (const ri_comm_xfer_fp_t reply_fp,
                                        const uint8_t * const raw_message)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_comm_message_t msg = {0};
    msg.data_length = RE_STANDARD_MESSAGE_LENGTH;
    msg.data[RE_STANDARD_DESTINATION_INDEX] = raw_message[RE_STANDARD_SOURCE_INDEX];
    msg.data[RE_STANDARD_SOURCE_INDEX] = raw_message[RE_STANDARD_DESTINATION_INDEX];
    msg.data[RE_STANDARD_OPERATION_INDEX] = RE_STANDARD_LOG_VALUE_WRITE;
    memset (&msg.data[RE_STANDARD_HEADER_LENGTH], 0xFF, RE_STANDARD_PAYLOAD_LENGTH);
    app_comms_blocking_send (reply_fp, &msg);
    return err_code;
}

/**
 * @brief Send heartbeat overdue data message.
 * TODO -refactor encoding to endpoints.
 * TODO -refactor into comms
 */
static rd_status_t app_sensor_send_timeout (const ri_comm_xfer_fp_t reply_fp,
        const uint8_t * const raw_message)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_comm_message_t msg = {0};
    msg.data_length = RE_STANDARD_MESSAGE_LENGTH;
    msg.data[RE_STANDARD_DESTINATION_INDEX] = raw_message[RE_STANDARD_SOURCE_INDEX];
    msg.data[RE_STANDARD_SOURCE_INDEX] = raw_message[RE_STANDARD_DESTINATION_INDEX];
    msg.data[RE_STANDARD_OPERATION_INDEX] = RE_STANDARD_OP_TIMEOUT;
    memset (&msg.data[RE_STANDARD_HEADER_LENGTH], 0xFF, RE_STANDARD_PAYLOAD_LENGTH);
    app_comms_blocking_send (reply_fp, &msg);
    return err_code;
}

/**
 * @brief Log read sensor op.
 *
 * @ref sensor_op.
 *
 * @param[in] reply_fp Function pointer to which send logs.
 * @param[fields] Fields to read.
 * @param[raw_message] Original message from remote.
 * @retval RD_SUCCESS on success.
 * @retval RD_ERROR_INVALID_PARAM if start of logs is after current time.
 * @retval error code from reply_fp if reply fails.
 *
 * @note This function blocks until all requested logs are sent and will therefore
 *       block for a long time.
 */
static rd_status_t app_sensor_log_read (const ri_comm_xfer_fp_t reply_fp,
                                        const rd_sensor_data_fields_t fields,
                                        const uint8_t * const raw_message)
{
    rd_status_t err_code = RD_SUCCESS;
    rd_sensor_data_t sample = {0};
    sample.fields = fields;
    float data[rd_sensor_data_fieldcount (&sample)];
    sample.data = data;
    // Parse start, end times.
    int64_t current_time_s = (int64_t) re_std_log_current_time (raw_message);
    int64_t start_s = (int64_t) re_std_log_start_time (raw_message);
    uint32_t sent_elements = 0;
    // overflow in 292 277 266 years
    const int64_t system_time_ms = (int64_t) ri_rtc_millis();

    // Cannot have start_s >= current_time_s
    if (current_time_s > start_s)
    {
        // Parse offset to system clock
        LOG ("Sending logged data\r\n");
        int64_t system_time_s = (system_time_ms / 1000LL);
        int64_t offset_ms = (current_time_s - system_time_s)  * 1000LL;
        int64_t time_diff_ms = (current_time_s - start_s) * 1000LL;
        // First sample to send in real time
        sample.timestamp_ms = (start_s * 1000LL);

        // Offset sample time to system clock.
        if (time_diff_ms > system_time_ms)
        {
            sample.timestamp_ms = 0;
        }
        else
        {
            sample.timestamp_ms = system_time_ms - time_diff_ms;
        }

        app_log_read_state_t rs =
        {
            .oldest_element_ms = sample.timestamp_ms,
            .element_idx = 0,
            .page_idx = 0
        };

        while (RD_SUCCESS == err_code)
        {
            // Reset data validity
            sample.valid.bitfield = 0;
            // Timestamp and fields are set in log read function.
            err_code |= app_log_read (&sample, &rs);

            if (RD_ERROR_NOT_FOUND == err_code)
            {
                err_code |= app_sensor_send_eof (reply_fp, raw_message);
                char msg[128];
                snprintf (msg, sizeof (msg), "Logged data sent: %lu elements\r\n", sent_elements); //-V576
                LOG (msg);
                sent_elements = 0;
            }
            else if (app_heartbeat_overdue())
            {
                err_code |= RD_ERROR_TIMEOUT;
                err_code |= app_sensor_send_timeout (reply_fp, raw_message);
            }
            // If data element was found, send log element.
            else if (RD_SUCCESS == err_code)
            {
                err_code |= app_sensor_send_data (reply_fp, raw_message,
                                                  &sample, offset_ms);
                sent_elements++;
                LOGD ("S");
            }
            else
            {
                // No action needed, error code gets returned to caller.
            }
        }
    }
    // Start time >= current time
    else
    {
        err_code |= RD_ERROR_INVALID_PARAM;
    }

    // Send op status, remove expected not found
    err_code &= ~RD_ERROR_NOT_FOUND;
    return err_code;
}

rd_status_t app_sensor_handle (const ri_comm_xfer_fp_t reply_fp,
                               const uint8_t * const raw_message,
                               const uint16_t data_len)
{
    rd_status_t err_code = RD_SUCCESS;

    if (NULL == raw_message)
    {
        err_code |= RD_ERROR_NULL;
    }
    else if (data_len < RE_STANDARD_MESSAGE_LENGTH)
    {
        err_code |= RD_ERROR_DATA_SIZE;
    }
    else
    {
        // Parse affected fields. It's ok to have unspecified value here,
        // in that case fields end up being empty and error is reported via reply_fp.
        re_type_t type = (re_type_t) raw_message[RE_STANDARD_DESTINATION_INDEX];
        rd_sensor_data_fields_t target_fields = re2rd_fields (type);
        // Parse desired operation.
        re_op_t op = (re_op_t) raw_message[RE_STANDARD_OPERATION_INDEX];

        // If target and op are valid, execute.
        switch (op)
        {
            case RE_STANDARD_LOG_VALUE_READ:
                err_code |= app_sensor_log_read (reply_fp,
                                                 target_fields, raw_message);
                break;

            default:
                // Reply with error on unknown op.
                break;
        }
    }

    return err_code;
}

rd_status_t app_sensor_vdd_sample (void)
{
    rd_status_t err_code = RD_SUCCESS;
    rd_sensor_configuration_t configuration =
    {
        .dsp_function = RD_SENSOR_CFG_DEFAULT,
        .dsp_parameter = RD_SENSOR_CFG_DEFAULT,
        .mode = RD_SENSOR_CFG_SINGLE,
        .resolution = RD_SENSOR_CFG_DEFAULT,
        .samplerate = RD_SENSOR_CFG_DEFAULT,
        .scale = RD_SENSOR_CFG_DEFAULT
    };
    err_code |= rt_adc_vdd_prepare (&configuration);
    err_code |= rt_adc_vdd_sample();
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