#ifndef APP_SENSOR_H
#define APP_SENSOR_H
/**
 * @addtogroup app
 */
/** @{ */
/**
 * @defgroup app_sensor Application sensor control
 * @brief Initialize, configure and read sensors.
 */
/** @} */
/**
 * @addtogroup app_sensor
 */
/** @{ */
/**
 * @file app_sensor.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-04-16
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Typical usage:
 * @code{.c}
 * TODO
 * @endcode
 */

#include "app_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_task_sensor.h"
#include "ruuvi_interface_environmental_mcu.h"
#include "ruuvi_interface_tmp117.h"

#define APP_SENSOR_SELFTEST_RETRIES (5U) //!< Number of times to retry init on self-test fail.
#define APP_SENSOR_HANDLE_UNUSED    RD_HANDLE_UNUSED

enum
{
#if APP_SENSOR_TMP117_ENABLED
    TMP117_INDEX,
#endif
#if APP_SENSOR_SHTCX_ENABLED
    SHTCX_INDEX,
#endif
#if APP_SENSOR_DPS310_ENABLED
    DPS310_INDEX,
#endif
#if APP_SENSOR_BME280_ENABLED
    BME280_INDEX,
#endif
#if APP_SENSOR_NTC_ENABLED
    NTC_INDEX,
#endif
#if APP_SENSOR_PHOTO_ENABLED
    PHOTO_INDEX,
#endif
#if APP_SENSOR_ENVIRONMENTAL_MCU_ENABLED
    ENV_MCU_INDEX,
#endif
#if APP_SENSOR_LIS2DH12_ENABLED
    LIS2DH12_INDEX,
#endif
#if APP_SENSOR_LIS2DW12_ENABLED
    LIS2DW12_INDEX,
#endif
    SENSOR_COUNT
};

#ifdef CEEDLING
void m_sensors_init (void); //!< Give Ceedling a handle to initialize structs.
#endif

#if APP_SENSOR_BME280_ENABLED
#if RB_ENVIRONMENTAL_BME280_SPI_USE
#define BME_BUS RD_BUS_SPI
#define BME_HANDLE RB_SPI_SS_ENVIRONMENTAL_PIN
#elif RB_ENVIRONMENTAL_BME280_I2C_USE
#define BME_BUS RD_BUS_I2C
#define BME_HANDLE RB_BME280_I2C_ADDRESS
#else
#error "No bus defined for BME280"
#endif

#define APP_SENSOR_BME280_DEFAULT_CFG                     \
  {                                                       \
    .sensor = {0},                                        \
    .init = &ri_bme280_init,                              \
    .configuration =                                      \
        {                                                 \
            .dsp_function = APP_SENSOR_BME280_DSP_FUNC,   \
            .dsp_parameter = APP_SENSOR_BME280_DSP_PARAM, \
            .mode = APP_SENSOR_BME280_MODE,               \
            .resolution = APP_SENSOR_BME280_RESOLUTION,   \
            .samplerate = APP_SENSOR_BME280_SAMPLERATE,   \
            .scale = APP_SENSOR_BME280_SCALE},            \
    .nvm_file = APP_FLASH_SENSOR_FILE,                    \
    .nvm_record = APP_FLASH_SENSOR_BME280_RECORD,         \
    .bus = BME_BUS,                                       \
    .handle = BME_HANDLE,                                 \
    .pwr_pin = RB_BME280_SENSOR_POWER_PIN,                \
    .pwr_on = RI_GPIO_HIGH,                               \
    .fifo_pin = RI_GPIO_ID_UNUSED,                        \
    .level_pin = RI_GPIO_ID_UNUSED                        \
  }
#endif

#if APP_SENSOR_DPS310_ENABLED
#define APP_SENSOR_DPS310_DEFAULT_CFG                     \
  {                                                       \
    .sensor = {0},                                        \
    .init = &ri_dps310_init,                              \
    .configuration =                                      \
        {                                                 \
            .dsp_function = APP_SENSOR_DPS310_DSP_FUNC,   \
            .dsp_parameter = APP_SENSOR_DPS310_DSP_PARAM, \
            .mode = APP_SENSOR_DPS310_MODE,               \
            .resolution = APP_SENSOR_DPS310_RESOLUTION,   \
            .samplerate = APP_SENSOR_DPS310_SAMPLERATE,   \
            .scale = APP_SENSOR_DPS310_SCALE},            \
    .nvm_file = APP_FLASH_SENSOR_FILE,                    \
    .nvm_record = APP_FLASH_SENSOR_DPS310_RECORD,         \
    .bus = RD_BUS_SPI,                                    \
    .handle = RB_SPI_SS_ENVIRONMENTAL_PIN,                \
    .pwr_pin = RB_DPS310_SENSOR_POWER_PIN,                \
    .pwr_on = RI_GPIO_HIGH,                               \
    .fifo_pin = RI_GPIO_ID_UNUSED,                        \
    .level_pin = RI_GPIO_ID_UNUSED                        \
  }
#endif

#if APP_SENSOR_LIS2DH12_ENABLED
#define APP_SENSOR_LIS2DH12_DEFAULT_CFG                     \
  {                                                         \
    .sensor = {0},                                          \
    .init = &ri_lis2dh12_init,                              \
    .configuration =                                        \
        {                                                   \
            .dsp_function = APP_SENSOR_LIS2DH12_DSP_FUNC,   \
            .dsp_parameter = APP_SENSOR_LIS2DH12_DSP_PARAM, \
            .mode = APP_SENSOR_LIS2DH12_MODE,               \
            .resolution = APP_SENSOR_LIS2DH12_RESOLUTION,   \
            .samplerate = APP_SENSOR_LIS2DH12_SAMPLERATE,   \
            .scale = APP_SENSOR_LIS2DH12_SCALE},            \
    .nvm_file = APP_FLASH_SENSOR_FILE,                      \
    .nvm_record = APP_FLASH_SENSOR_LIS2DH12_RECORD,         \
    .bus = RD_BUS_SPI,                                      \
    .handle = RB_SPI_SS_ACCELEROMETER_PIN,                  \
    .pwr_pin = RB_LIS2DH12_SENSOR_POWER_PIN,                \
    .pwr_on = RI_GPIO_HIGH,                                 \
    .fifo_pin = RB_INT_FIFO_PIN,                            \
    .level_pin = RB_INT_LEVEL_PIN                           \
  }
#endif

#if APP_SENSOR_LIS2DW12_ENABLED
#define APP_SENSOR_LIS2DW2_DEFAULT_CFG                      \
  {                                                         \
    .sensor = {0},                                          \
    .init = ri_lis2dw12_init,                               \
    .configuration = {0},                                   \
    .nvm_file = APPLICATION_FLASH_SENSOR_FILE,              \
    .nvm_record = APPLICATION_FLASH_SENSOR_LIS2DW12_RECORD, \
    .bus = RD_BUS_SPI,                                      \
    .handle = RB_SPI_SS_ACCELEROMETER_PIN,                  \
    .pwr_pin = RI_GPIO_ID_UNUSED,                           \
    .pwr_on = RI_GPIO_HIGH,                                 \
    .fifo_pin = RB_INT_ACC1_PIN,                            \
    .level_pin = RB_INT_ACC2_PIN                            \
  }
#endif

#if APP_SENSOR_SHTCX_ENABLED
#define APP_SENSOR_SHTCX_DEFAULT_CFG                     \
  {                                                      \
    .sensor = {0},                                       \
    .init = &ri_shtcx_init,                              \
    .configuration =                                     \
        {                                                \
            .dsp_function = APP_SENSOR_SHTCX_DSP_FUNC,   \
            .dsp_parameter = APP_SENSOR_SHTCX_DSP_PARAM, \
            .mode = APP_SENSOR_SHTCX_MODE,               \
            .resolution = APP_SENSOR_SHTCX_RESOLUTION,   \
            .samplerate = APP_SENSOR_SHTCX_SAMPLERATE,   \
            .scale = APP_SENSOR_SHTCX_SCALE},            \
    .nvm_file = APP_FLASH_SENSOR_FILE,                   \
    .nvm_record = APP_FLASH_SENSOR_SHTCX_RECORD,         \
    .bus = RD_BUS_I2C,                                   \
    .handle = RB_SHTCX_I2C_ADDRESS,                      \
    .pwr_pin = RB_SHTCX_SENSOR_POWER_PIN,                \
    .pwr_on = RI_GPIO_HIGH,                              \
    .fifo_pin = RI_GPIO_ID_UNUSED,                       \
    .level_pin = RI_GPIO_ID_UNUSED                       \
  }
#endif

#if APP_SENSOR_TMP117_ENABLED
#define APP_SENSOR_TMP117_DEFAULT_CFG                     \
  {                                                       \
    .sensor = {0},                                        \
    .init = &ri_tmp117_init,                              \
    .configuration =                                      \
    {                                                     \
            .dsp_function = APP_SENSOR_TMP117_DSP_FUNC,   \
            .dsp_parameter = APP_SENSOR_TMP117_DSP_PARAM, \
            .mode = APP_SENSOR_TMP117_MODE,               \
            .resolution = APP_SENSOR_TMP117_RESOLUTION,   \
            .samplerate = APP_SENSOR_TMP117_SAMPLERATE,   \
            .scale = APP_SENSOR_TMP117_SCALE              \
    },                                                    \
    .nvm_file = APP_FLASH_SENSOR_FILE,                    \
    .nvm_record = APP_FLASH_SENSOR_TMP117_RECORD,         \
    .bus = RD_BUS_I2C,                                    \
    .handle = RB_TMP117_I2C_ADDRESS,                      \
    .pwr_pin = RB_TMP117_SENSOR_POWER_PIN,                \
    .pwr_on = RI_GPIO_HIGH,                               \
    .fifo_pin = RI_GPIO_ID_UNUSED,                        \
    .level_pin = RI_GPIO_ID_UNUSED                        \
  }
#endif

#if APP_SENSOR_PHOTO_ENABLED
#define APP_SENSOR_PHOTO_DEFAULT_CFG             \
  {                                              \
    .sensor = {0},                               \
    .init = &ri_adc_photo_init,                  \
    .configuration = {0},                        \
    .nvm_file = APP_FLASH_SENSOR_FILE,           \
    .nvm_record = APP_FLASH_SENSOR_PHOTO_RECORD, \
    .bus = RD_BUS_NONE,                          \
    .handle = RB_PHOTO_ADC,                      \
    .pwr_pin = RB_PHOTO_PWR_PIN,                 \
    .pwr_on = RB_PHOTO_ACTIVE,                   \
    .fifo_pin = RI_GPIO_ID_UNUSED,               \
    .level_pin = RI_GPIO_ID_UNUSED               \
  }
#endif

#if APP_SENSOR_NTC_ENABLED
#define APP_SENSOR_NTC_DEFAULT_CFG             \
  {                                            \
    .sensor = {0},                             \
    .init = &ri_adc_ntc_init,                  \
    .configuration = {0},                      \
    .nvm_file = APP_FLASH_SENSOR_FILE,         \
    .nvm_record = APP_FLASH_SENSOR_NTC_RECORD, \
    .bus = RD_BUS_NONE,                        \
    .handle = RB_NTC_ADC,                      \
    .pwr_pin = RB_NTC_PWR_PIN,                 \
    .pwr_on = RB_NTC_ACTIVE,                   \
    .fifo_pin = RI_GPIO_ID_UNUSED,             \
    .level_pin = RI_GPIO_ID_UNUSED             \
  }
#endif

#if APP_SENSOR_ENVIRONMENTAL_MCU_ENABLED
#define APP_SENSOR_ENVIRONMENTAL_MCU_DEFAULT_CFG         \
  {                                                      \
    .sensor = {0},                                       \
    .init = &ri_environmental_mcu_init,                  \
    .configuration =                                     \
    {                                                    \
            .dsp_function = APP_SENSOR_NRF52_DSP_FUNC,   \
            .dsp_parameter = APP_SENSOR_NRF52_DSP_PARAM, \
            .mode = APP_SENSOR_NRF52_MODE,               \
            .resolution = APP_SENSOR_NRF52_RESOLUTION,   \
            .samplerate = APP_SENSOR_NRF52_SAMPLERATE,   \
            .scale = APP_SENSOR_NRF52_SCALE              \
    },                                                   \
    .nvm_file = APP_FLASH_SENSOR_FILE,                   \
    .nvm_record = APP_FLASH_SENSOR_ENVI_RECORD,          \
    .bus = RD_BUS_NONE,                                  \
    .handle = RD_BUS_NONE,                               \
    .pwr_pin = RI_GPIO_ID_UNUSED,                        \
    .pwr_on = RI_GPIO_LOW,                               \
    .fifo_pin = RI_GPIO_ID_UNUSED,                       \
    .level_pin = RI_GPIO_ID_UNUSED                       \
  }
#endif

/**
 * @brief Initialize sensors into default mode or to a mode stored to flash.
 *
 * This function checks app_config.h for enabled sensors and initializes each of them
 * into mode stored into flash, or into default mode defined in app_config.h if the
 * mode is not stored in flash. Internal sampling rate of sensors does not affect
 * reading rate of sensors, reading rate must be configured separately.
 *
 * @retval RD_SUCCESS on success, NOT_FOUND sensors are allowed.
 * @retval RD_ERROR_INVALID_STATE if GPIO or GPIO interrupts are not enabled.
 * @retval RD_ERROR_SELFTEST if sensor is found on the bus and fails selftest.
 */
rd_status_t app_sensor_init (void);

/**
 * @brief Configure sampling of sensors.
 *
 * This function lets application know what data and how often should be read.
 * To use sampled data, call @ref app_sensor_get.
 *
 * @param[in] data Data fields to sample.
 * @param[in] interval_ms Interval to sample at. At minimum 1000 ms.
 *
 * @retval RD_SUCCESS on success.
 * @retval RD_ERROR_INVALID_PARAM if interval is less than 1000 ms.
 *
 * @note: Future minor version may allow using intervals down to 1 ms on
 *        sensors with built-in FIFOs and read the FIFO in batches.
 *
 */
rd_status_t app_sensor_sample_config (const rd_sensor_data_fields_t data,
                                      const uint32_t interval_ms);

/**
 * @brief Return available data types.
 *
 * @note This is refreshed from sensor structs RAM which makes
 * this a relatively expensive function call due to looping over
 * all sensor contexts. Cache this if microseconds count in your application.
 *
 * @return Listing of data the application can provide.
 */
rd_sensor_data_fields_t app_sensor_available_data (void);

/**
 * @brief Return last sampled data.
 *
 * This function checks loops through initialized sensors until all data in
 * data->fields is valid or all sensors are checked.
 *
 * @retval RD_SUCCESS on success, NOT_FOUND sensors are allowed.
 * @retval RD_ERROR_SELFTEST if sensor is found on the bus and fails selftest.
 */
rd_status_t app_sensor_get (rd_sensor_data_t * const data);

/**
 * @brief Uninitialize sensors into low-power mode.
 *
 * @retval RD_SUCCESS On success.
 * @return Error code from stack on error.
 */
rd_status_t app_sensor_uninit (void);

/**
 * @brief Uninitialize sensors into low-power mode.
 *
 * @retval RD_SUCCESS On success.
 * @return Error code from stack on error.
 */
rd_status_t app_sensor_uninit (void);

/**
 * @brief Find and return a sensor which can provide requested data.
 *
 * Loops through sensors in order of priority, if board has SHTC temperature and
 * humidity sensor and LIS2DH12 acceleration and temperature sensor, searching
 * for the sensor will return the one which is first in m_sensors list.
 *
 * Works only witjh initialized sensors, will not return a sensor which is supported
 * in firmawre but not initialized due to self-test error etc.
 *
 * @param[in] data fields which sensor must provide.
 * @return Pointer to SENSOR, NULL if suitable sensor is not found.
 * @note If parameter data is empty, first initialized sensor will be returned.
 */
rd_sensor_t * app_sensor_find_provider (const rd_sensor_data_fields_t data);

/**
 * @brief Increment event counter of application. Rolls over at 2^32.
 */
void app_sensor_event_increment (void);

/**
 * @brief Get current event count.
 *
 * @return Number of events accumulated, rolls over at int32_t.
 */
uint32_t app_sensor_event_count_get (void);

/**
 * @brief Set threshold for accelerometer interrupts.
 *
 * Accelerometers are high-passed so gravity won't affect given threshold.
 * Acceleration event is triggered when the threshold is exceeded on any axis.
 * Acceleration event ceases when acceleration falls below the threshold, and
 * can then be triggered again. Maximum rate for acceleration events is then
 * accelerometer sample rate divided by two.
 *
 * On acceleration event @ref app_sensor_event_increment is called.
 *
 * @param[in, out] threshold_g In: Thershold of acceleration, > 0. Interpreted as
 *                                 "at least this much". NULL to disable interrupts.
 *                             Out: Configured threshold.
 * @retval RD_SUCCESS if threshold was configured.
 * @retval RD_ERROR_NOT_IMPLEMENTED if threshold is lower than 0 (negative).
 * @retval RD_ERROR_NOT_SUPPORTED if no suitable accelerometer is initialized.
 *
 *
 */
rd_status_t app_sensor_acc_thr_set (float * threshold_g);

/**
 * @brief Handle data coming in to the application.
 *
 * Interprets the desired action, executes it and aknowledges to
 * reply_fp with ri_comm_message_t containing ruuvi endpoint encoded message.
 *
 * For example a Log Read command gets replied with all the logged elements.
 *
 *
 * @param[in] ri_reply_fp Function pointer to send replies to.
 * @param[in] raw_message Payload sent by client. Must be a standard Ruuvi Endpoint
 *                        message.
 * @param[in] data_len Payload sent by client.
 *
 * @retval RD_SUCCESS Data was handled, including replying with error to reply_fp.
 * @retval RD_ERROR_NULL Raw message or replyfp is NULL.
 * @retval RD_ERROR_DATA_SIZE data_len is less than RE_STANDARD_MESSAGE_LENGTH.
 *
 * @warning Executing these commands can be resource intensive. Consider
 *          stopping app_heartbeat before entering this function.
 *
 */
rd_status_t app_sensor_handle (const ri_comm_xfer_fp_t ri_reply_fp,
                               const uint8_t * const raw_message,
                               const uint16_t data_len);

/**
 * @brief Synchronize VDD measurement to radio activity.
 *
 * Call this function before and after radio events to notify
 * the device that ADC should be prepared to sample the droop voltage
 * and to take the sample after heavy loading.
 *
 * @param[in] evt Type of next radio event, RI_RADIO_BEFORE ot RI_RADIO_AFTER
 */
void app_sensor_vdd_measure_isr (const ri_radio_activity_evt_t evt);

/**
 * @brief Prepare VDD and take a sample
 *
 * Call this function during initialization process to take a VDD sample
 * for the first heartbeat message.
 *
 * @retval RD_SUCCESS on success
 * @retval RD_ERROR_INVALID_STATE if ADC is not prepared
 */
rd_status_t app_sensor_vdd_sample (void);

#ifdef RUUVI_RUN_TESTS
void app_sensor_ctx_get (rt_sensor_ctx_t *** m_sensors, size_t * num_sensors);
#endif

#ifdef CEEDLING
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_gpio_interrupt.h"
void on_radio_isr (const ri_radio_activity_evt_t evt);
void on_accelerometer_isr (const ri_gpio_evt_t event);
#endif

#endif
/** @}*/
