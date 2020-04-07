#include "application_config.h"
#include "data_structures/ruuvi_library_ringbuffer.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_interface_atomic.h"
#include "ruuvi_interface_bme280.h"
#include "ruuvi_interface_communication.h"
#include "ruuvi_interface_environmental_mcu.h"
#include "ruuvi_interface_lis2dh12.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_shtcx.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_tmp117.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_yield.h"
#include "task_communication.h"
#include "task_environmental.h"
#include "task_flash.h"
#include "task_rtc.h"
#include "task_sensor.h"
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifndef TASK_ENVIRONMENTAL_LOG_LEVEL
#define TASK_ENVIRONMENTAL_LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#endif

#define LOG(msg) ruuvi_interface_log(TASK_ENVIRONMENTAL_LOG_LEVEL, msg)
#define LOGD(msg) ruuvi_interface_log(RUUVI_INTERFACE_LOG_DEBUG, msg)
#define LOGW(msg) ruuvi_interface_log(RUUVI_INTERFACE_LOG_WARNING, msg)
#define LOGHEX(msg, len) ruuvi_interface_log_hex(TASK_ENVIRONMENTAL_LOG_LEVEL, msg, len)

//static ruuvi_interface_timer_id_t m_log_timer;               //!< Timer for logging data

// Do not compile space for unused sensor drivers.
// Define enum in order of default preference of sensor being used.
// Default sensor can be overridden by calling a backend_set function.
enum
{
#if APPLICATION_ENVIRONMENTAL_TMP117_ENABLED
    ENV_TMP117_INDEX,
#endif
#if APPLICATION_ENVIRONMENTAL_SHTCX_ENABLED
    ENV_SHTCX_INDEX,
#endif
#if APPLICATION_ENVIRONMENTAL_BME280_ENABLED
    ENV_BME280_INDEX,
#endif
#if APPLICATION_ENVIRONMENTAL_NTC_ENABLED
    ENV_NTC_INDEX,
#endif
#if APPLICATION_ENVIRONMENTAL_MCU_ENABLED
    ENV_MCU_INDEX,
#endif
#if APPLICATION_ENVIRONMENTAL_LIS2DH12_ENABLED
    ENV_LIS2DH12_INDEX,
#endif
    ENV_SENSOR_COUNT
};
static ruuvi_driver_sensor_t  m_environmental_sensors[ENV_SENSOR_COUNT] = {0}; //!< Sensor APIs.
static ruuvi_driver_sensor_t * m_active_sensor =
    NULL; //!< Sensor being used by application.

static uint8_t
buffer[APPLICATION_ENVIRONMENTAL_RAMBUFFER_SIZE]; //!< Raw buffer for environmental logs.
static ruuvi_interface_atomic_t buffer_wlock; //!< Lock for writing to ringbuffer.
static ruuvi_interface_atomic_t buffer_rlock; //!< Lock for reading from ringbuffer.
/** @brief Buffer structure for outgoing data */
static ruuvi_library_ringbuffer_t ringbuf = {.head = 0,
                                             .tail = 0,
                                             .block_size = sizeof (environmental_log_t),
                                             .storage_size = sizeof (buffer),
                                             .index_mask = (sizeof (buffer) / sizeof (environmental_log_t)) - 1,
                                             .storage = buffer,
                                             .lock = ruuvi_interface_atomic_flag,
                                             .writelock = &buffer_wlock,
                                             .readlock  = &buffer_rlock
                                            };

static task_communication_api_t environmental_api =
{
    .sensor      = &m_active_sensor,
    .offset_set  = NULL,
    .offset_get  = NULL,
    .data_target = NULL,
    .log_cfg     = NULL,
    .log_read    = task_environmental_log_read
};

ruuvi_driver_status_t task_environmental_api_get (task_communication_api_t ** api)
{
    if (api == NULL)
    {
        return RUUVI_DRIVER_ERROR_NULL;
    }

    *api = &environmental_api;
    LOGD ("Returned API \r\n");
    return RUUVI_DRIVER_SUCCESS;
}

/** @brief Try to initialize SHTCX sensor
 *
 * Looks up appropriate pin definitions from ruuvi_boards.h
 * Tries to load driver configuration from flash. If flash configuration is not available,
 * uses application defaults from application_config.h.
 *
 * @return RUUVI_DRIVER_SUCCESS if SHTCX is not enabled at compile time or if sensor is initialized.
 * @return RUUVI_DRIVER_ERROR_NOT_FOUND if SHTCX does not reply on bus but it's expected to be available
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if some other user has already initialized the driver.
 */
static ruuvi_driver_status_t initialize_shtcx (void)
{
#if APPLICATION_ENVIRONMENTAL_SHTCX_ENABLED
    // Assume "Not found", gets set to "Success" if a usable sensor is present
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;
    ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_I2C;
    uint8_t handle = RUUVI_BOARD_SHTCX_I2C_ADDRESS;
    // Initialize sensor.
    err_code = ruuvi_interface_shtcx_init (& (m_environmental_sensors[ENV_SHTCX_INDEX]),
                                           bus, handle);

    // return if failed.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        return err_code;
    }

    // Wait for flash operation to finish
    while (task_flash_busy());

    ruuvi_driver_sensor_configuration_t config;
    err_code = task_flash_load (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                APPLICATION_FLASH_ENVIRONMENTAL_SHTCX_RECORD,
                                &config,
                                sizeof (config));

    // If there is no stored configuration, use defaults. Skip
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        LOG ("SHTCX config not found on flash, using defaults\r\n");
        config.dsp_function  = APPLICATION_ENVIRONMENTAL_SHTCX_DSP_FUNC;
        config.dsp_parameter = APPLICATION_ENVIRONMENTAL_SHTCX_DSP_PARAM;
        config.mode          = APPLICATION_ENVIRONMENTAL_SHTCX_MODE;
        config.resolution    = APPLICATION_ENVIRONMENTAL_SHTCX_RESOLUTION;
        config.samplerate    = APPLICATION_ENVIRONMENTAL_SHTCX_SAMPLERATE;
        config.scale         = APPLICATION_ENVIRONMENTAL_SHTCX_SCALE;
        // Store defaults to flash

        if (RUUVI_DRIVER_ERROR_NOT_SUPPORTED != err_code)
        {
            err_code = task_flash_store (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                         APPLICATION_FLASH_ENVIRONMENTAL_SHTCX_RECORD,
                                         &config,
                                         sizeof (config));
        }
    }

    // Check flash operation status, allow not supported in case we're on 811
    RUUVI_DRIVER_ERROR_CHECK (err_code, RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

    // Wait for flash operation to finish
    while (task_flash_busy());

    // Configure sensor
    return task_sensor_configure (& (m_environmental_sensors[ENV_SHTCX_INDEX]), &config, "");
#else
    return RUUVI_DRIVER_SUCCESS;
#endif
}

/** @brief Try to initialize BME280 sensor
 *
 * Looks up appropriate pin definitions from ruuvi_boards.h
 * Tries to load driver configuration from flash. If flash configuration is not available,
 * uses application defaults from application_config.h.
 *
 * @return RUUVI_DRIVER_SUCCESS if BME280 is not enabled at compile time or if sensor is initialized.
 * @return RUUVI_DRIVER_ERROR_NOT_FOUND if BME280 does not reply on bus but it's expected to be available
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if some other user has already initialized the driver.
 */
static ruuvi_driver_status_t initialize_bme280 (void)
{
#if APPLICATION_ENVIRONMENTAL_BME280_ENABLED
    // Assume "Not found", gets set to "Success" if a usable sensor is present
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;
    ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_FAIL;
    uint8_t handle = 0;

    if (RUUVI_BOARD_ENVIRONMENTAL_BME280_SPI_USE)
    {
        bus = RUUVI_DRIVER_BUS_SPI;
        handle = RUUVI_BOARD_SPI_SS_ENVIRONMENTAL_PIN;
    }
    else if (RUUVI_BOARD_ENVIRONMENTAL_BME280_I2C_USE)
    {
        bus = RUUVI_DRIVER_BUS_I2C;
        handle = RUUVI_BOARD_BME280_I2C_ADDRESS;
    }
    else
    {
        return RUUVI_DRIVER_ERROR_NOT_SUPPORTED;
    }

    // Initialize sensor.
    err_code = ruuvi_interface_bme280_init (& (m_environmental_sensors[ENV_BME280_INDEX]),
                                            bus, handle);

    // return if failed.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        return err_code;
    }

    // Wait for flash operation to finish
    while (task_flash_busy());

    ruuvi_driver_sensor_configuration_t config;
    err_code = task_flash_load (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                APPLICATION_FLASH_ENVIRONMENTAL_BME280_RECORD,
                                &config,
                                sizeof (config));

    // If there is no stored configuration, use defaults.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        LOG ("BME280 config not found on flash, using defaults\r\n");
        config.dsp_function  = APPLICATION_ENVIRONMENTAL_BME280_DSP_FUNC;
        config.dsp_parameter = APPLICATION_ENVIRONMENTAL_BME280_DSP_PARAM;
        config.mode          = APPLICATION_ENVIRONMENTAL_BME280_MODE;
        config.resolution    = APPLICATION_ENVIRONMENTAL_BME280_RESOLUTION;
        config.samplerate    = APPLICATION_ENVIRONMENTAL_BME280_SAMPLERATE;
        config.scale         = APPLICATION_ENVIRONMENTAL_BME280_SCALE;

        // Store defaults to flash
        if (RUUVI_DRIVER_ERROR_NOT_SUPPORTED != err_code)
        {
            err_code = task_flash_store (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                         APPLICATION_FLASH_ENVIRONMENTAL_BME280_RECORD,
                                         &config,
                                         sizeof (config));
        }
    }

    // Check flash operation status, allow not supported in case we're on 811
    RUUVI_DRIVER_ERROR_CHECK (err_code, RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

    // Wait for flash operation to finish
    while (task_flash_busy());

    // Configure sensor
    return task_sensor_configure (& (m_environmental_sensors[ENV_BME280_INDEX]), &config, "");
#else
    return RUUVI_DRIVER_SUCCESS;
#endif
}

/** @brief Try to initialize NTC sensor
 *
 * Looks up appropriate pin definitions from ruuvi_boards.h
 * Tries to load driver configuration from flash. If flash configuration is not available,
 * uses application defaults from application_config.h.
 *
 * @return RUUVI_DRIVER_SUCCESS if NTC is not enabled at compile time or if sensor is initialized.
 * @return RUUVI_DRIVER_ERROR_NOT_FOUND if NTC does not reply on bus but it's expected to be available
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if some other user has already initialized the driver.
 */
static ruuvi_driver_status_t initialize_ntc (void)
{
#if APPLICATION_ENVIRONMENTAL_NTC_ENABLED
    // Assume "Not found", gets set to "Success" if a usable sensor is present
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;
    ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
    uint8_t handle = 0;
    // Initialize sensor.
    err_code = ruuvi_interface_environmental_ntc_init (&
               (m_environmental_sensors[ENV_NTC_INDEX]),
               bus, handle);

    // return if failed.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        return err_code;
    }

    // Wait for flash operation to finish
    while (task_flash_busy());

    ruuvi_driver_sensor_configuration_t config;
    err_code = task_flash_load (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                APPLICATION_FLASH_ENVIRONMENTAL_NTC_RECORD,
                                &config,
                                sizeof (config));

    // If there is no stored configuration, use defaults.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        LOG ("NTC config not found on flash, using defaults\r\n");
        config.dsp_function  = APPLICATION_ENVIRONMENTAL_NTC_DSP_FUNC;
        config.dsp_parameter = APPLICATION_ENVIRONMENTAL_NTC_DSP_PARAM;
        config.mode          = APPLICATION_ENVIRONMENTAL_NTC_MODE;
        config.resolution    = APPLICATION_ENVIRONMENTAL_NTC_RESOLUTION;
        config.samplerate    = APPLICATION_ENVIRONMENTAL_NTC_SAMPLERATE;
        config.scale         = APPLICATION_ENVIRONMENTAL_NTC_SCALE;

        // Store defaults to flash
        if (RUUVI_DRIVER_ERROR_NOT_SUPPORTED != err_code)
        {
            err_code = task_flash_store (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                         APPLICATION_FLASH_ENVIRONMENTAL_NTC_RECORD,
                                         &config,
                                         sizeof (config));
        }
    }

    // Check flash operation status, allow not supported in case we're on 811
    RUUVI_DRIVER_ERROR_CHECK (err_code, RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

    // Wait for flash operation to finish
    while (task_flash_busy());

    // Configure sensor
    return task_sensor_configure (& (m_environmental_sensors[ENV_NTC_INDEX]), &config, "");
#else
    return RUUVI_DRIVER_SUCCESS;
#endif
}

/** @brief Try to initialize MCU sensor
 *
 * Looks up appropriate pin definitions from ruuvi_boards.h
 * Tries to load driver configuration from flash. If flash configuration is not available,
 * uses application defaults from application_config.h.
 *
 * @return RUUVI_DRIVER_SUCCESS if MCU is not enabled at compile time or if sensor is initialized.
 * @return RUUVI_DRIVER_ERROR_NOT_FOUND if MCU does not reply on bus but it's expected to be available
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if some other user has already initialized the driver.
 */
static ruuvi_driver_status_t initialize_mcu (void)
{
#if APPLICATION_ENVIRONMENTAL_MCU_ENABLED
    // Assume "Not found", gets set to "Success" if a usable sensor is present
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;
    ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
    uint8_t handle = 0;
    // Initialize sensor.
    err_code = ruuvi_interface_environmental_mcu_init (&
               (m_environmental_sensors[ENV_MCU_INDEX]),
               bus, handle);

    // return if failed.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        return err_code;
    }

    // Wait for flash operation to finish
    while (task_flash_busy());

    ruuvi_driver_sensor_configuration_t config;
    err_code = task_flash_load (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                APPLICATION_FLASH_ENVIRONMENTAL_MCU_RECORD,
                                &config,
                                sizeof (config));

    // If there is no stored configuration, use defaults.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        LOG ("MCU Temp config not found on flash, using defaults\r\n");
        config.dsp_function  = APPLICATION_ENVIRONMENTAL_MCU_DSP_FUNC;
        config.dsp_parameter = APPLICATION_ENVIRONMENTAL_MCU_DSP_PARAM;
        config.mode          = APPLICATION_ENVIRONMENTAL_MCU_MODE;
        config.resolution    = APPLICATION_ENVIRONMENTAL_MCU_RESOLUTION;
        config.samplerate    = APPLICATION_ENVIRONMENTAL_MCU_SAMPLERATE;
        config.scale         = APPLICATION_ENVIRONMENTAL_MCU_SCALE;

        // Store defaults to flash
        if (RUUVI_DRIVER_ERROR_NOT_SUPPORTED != err_code)
        {
            err_code = task_flash_store (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                         APPLICATION_FLASH_ENVIRONMENTAL_MCU_RECORD,
                                         &config,
                                         sizeof (config));
        }
    }

    // Check flash operation status, allow not supported in case we're on 811
    RUUVI_DRIVER_ERROR_CHECK (err_code, RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

    // Wait for flash operation to finish
    while (task_flash_busy());

    // Configure sensor
    return task_sensor_configure (& (m_environmental_sensors[ENV_MCU_INDEX]), &config, "");
#else
    return RUUVI_DRIVER_SUCCESS;
#endif
}

/** @brief Try to initialize LIS2DH12 as environmental sensor
 *
 * Looks up appropriate pin definitions from ruuvi_boards.h
 * Tries to load driver configuration from flash. If flash configuration is not available,
 * uses application defaults from application_config.h.
 *
 * @return RUUVI_DRIVER_SUCCESS if LIS2DH12 environmental is not enabled at compile time or if sensor is initialized.
 * @return RUUVI_DRIVER_ERROR_NOT_FOUND if LIS2DH12 environmental does not reply on bus but it's expected to be available
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if some other user has already initialized the driver.
 */
static ruuvi_driver_status_t initialize_lis2dh12 (void)
{
#if APPLICATION_ENVIRONMENTAL_LIS2DH12_ENABLED
    // Assume "Not found", gets set to "Success" if a usable sensor is present
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;
    ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_FAIL;
    uint8_t handle = 0;

    if (RUUVI_BOARD_ACCELEROMETER_LIS2DH12_SPI_USE)
    {
        bus = RUUVI_DRIVER_BUS_SPI;
        handle = RUUVI_BOARD_SPI_SS_ACCELEROMETER_PIN;
    }
    else
    {
        return RUUVI_DRIVER_ERROR_NOT_IMPLEMENTED;
    }

    // Initialize sensor.
    err_code = ruuvi_interface_lis2dh12_init (& (m_environmental_sensors[ENV_LIS2DH12_INDEX]),
               bus, handle);

    // return if failed.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        return err_code;
    }

    // Wait for flash operation to finish
    while (task_flash_busy());

    ruuvi_driver_sensor_configuration_t config;
    err_code = task_flash_load (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                APPLICATION_FLASH_ENVIRONMENTAL_LIS2DH12_RECORD,
                                &config,
                                sizeof (config));

    // If there is no stored configuration, use defaults.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        LOG ("LIS2DH12 temp config not found on flash, using defaults\r\n");
        config.dsp_function  = APPLICATION_ENVIRONMENTAL_LIS2DH12_DSP_FUNC;
        config.dsp_parameter = APPLICATION_ENVIRONMENTAL_LIS2DH12_DSP_PARAM;
        config.mode          = APPLICATION_ENVIRONMENTAL_LIS2DH12_MODE;
        config.resolution    = APPLICATION_ENVIRONMENTAL_LIS2DH12_RESOLUTION;
        config.samplerate    = APPLICATION_ENVIRONMENTAL_LIS2DH12_SAMPLERATE;
        config.scale         = APPLICATION_ENVIRONMENTAL_LIS2DH12_SCALE;

        // Store defaults to flash
        if (RUUVI_DRIVER_ERROR_NOT_SUPPORTED != err_code)
        {
            err_code = task_flash_store (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                         APPLICATION_FLASH_ENVIRONMENTAL_LIS2DH12_RECORD,
                                         &config,
                                         sizeof (config));
        }
    }

    // Check flash operation status, allow not supported in case we're on 811
    RUUVI_DRIVER_ERROR_CHECK (err_code, RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

    // Wait for flash operation to finish
    while (task_flash_busy());

    // Configure sensor
    return task_sensor_configure (& (m_environmental_sensors[ENV_LIS2DH12_INDEX]), &config,
                                  "");
#else
    return RUUVI_DRIVER_SUCCESS;
#endif
}

/** @brief Try to initialize TMP117 as environmental sensor
 *
 * Looks up appropriate pin definitions from ruuvi_boards.h
 * Tries to load driver configuration from flash. If flash configuration is not available,
 * uses application defaults from application_config.h.
 *
 * @return RUUVI_DRIVER_SUCCESS if TMP117 environmental is not enabled at compile time or if sensor is initialized.
 * @return RUUVI_DRIVER_ERROR_NOT_FOUND if TMP117 environmental does not reply on bus but it's expected to be available
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if some other user has already initialized the driver.
 */
static ruuvi_driver_status_t initialize_tmp117 (void)
{
#if APPLICATION_ENVIRONMENTAL_TMP117_ENABLED
    // Assume "Not found", gets set to "Success" if a usable sensor is present
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_ERROR_NOT_FOUND;
    ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_I2C;
    uint8_t handle = RUUVI_BOARD_TMP117_I2C_ADDRESS;
    // Initialize sensor.
    err_code = ruuvi_interface_tmp117_init (& (m_environmental_sensors[ENV_TMP117_INDEX]),
                                            bus, handle);

    // return if failed.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        return err_code;
    }

    // Wait for flash operation to finish
    while (task_flash_busy());

    ruuvi_driver_sensor_configuration_t config;
    err_code = task_flash_load (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                APPLICATION_FLASH_ENVIRONMENTAL_TMP117_RECORD,
                                &config,
                                sizeof (config));

    // If there is no stored configuration, use defaults.
    if (RUUVI_DRIVER_SUCCESS != err_code)
    {
        LOG ("TMP117 config not found on flash, using defaults\r\n");
        config.dsp_function  = APPLICATION_ENVIRONMENTAL_TMP117_DSP_FUNC;
        config.dsp_parameter = APPLICATION_ENVIRONMENTAL_TMP117_DSP_PARAM;
        config.mode          = APPLICATION_ENVIRONMENTAL_TMP117_MODE;
        config.resolution    = APPLICATION_ENVIRONMENTAL_TMP117_RESOLUTION;
        config.samplerate    = APPLICATION_ENVIRONMENTAL_TMP117_SAMPLERATE;
        config.scale         = APPLICATION_ENVIRONMENTAL_TMP117_SCALE;

        // Store defaults to flash
        if (RUUVI_DRIVER_ERROR_NOT_SUPPORTED != err_code)
        {
            err_code = task_flash_store (APPLICATION_FLASH_ENVIRONMENTAL_FILE,
                                         APPLICATION_FLASH_ENVIRONMENTAL_TMP117_RECORD,
                                         &config,
                                         sizeof (config));
        }
    }

    // Check flash operation status, allow not supported in case we're on 811
    RUUVI_DRIVER_ERROR_CHECK (err_code, RUUVI_DRIVER_ERROR_NOT_SUPPORTED);

    // Wait for flash operation to finish
    while (task_flash_busy());

    // Configure sensor
    err_code |= task_sensor_configure (& (m_environmental_sensors[ENV_TMP117_INDEX]), &config,
                                       "");
    return err_code;
#else
    return RUUVI_DRIVER_SUCCESS;
#endif
}
/*
static void execute_log (void * event, uint16_t event_size)
{
    static uint32_t tick_count = 0;
    uint32_t interval_ms = APPLICATION_ENVIRONMENTAL_LOG_INTERVAL_MS;
    uint32_t tick_interval = APPLICATION_ENVIRONMENTAL_TICK_MS;
    const uint32_t ticks_per_log = interval_ms / tick_interval;

    if (0 == tick_count)
    {
        task_environmental_log();
    }

    tick_count = (ticks_per_log <= tick_count) ? 0 : (tick_count + 1);
}

static void schedule_log (void * p_context)
{
    ruuvi_interface_scheduler_event_put (NULL, 0, execute_log);
}
*/
ruuvi_driver_status_t task_environmental_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    // Attempt to initialize all possible temperature backends.
    err_code |= initialize_tmp117();
    err_code |= initialize_shtcx();
    err_code |= initialize_bme280();
    err_code |= initialize_ntc();
    err_code |= initialize_mcu();
    err_code |= initialize_lis2dh12();
    RUUVI_DRIVER_ERROR_CHECK (err_code, RUUVI_DRIVER_ERROR_NOT_FOUND);

    // Use first valid backend as the default provider.
    for (int ii = 0; ii < ENV_SENSOR_COUNT; ii++)
    {
        if (m_environmental_sensors[ii].provides.bitfield)
        {
            m_active_sensor = & (m_environmental_sensors[ii]);
            break;
        }
    }

    // XXX - use generic logging
    /*
    if (NULL == m_log_timer)
    {
        ruuvi_interface_timer_create (&m_log_timer, RUUVI_INTERFACE_TIMER_MODE_REPEATED,
                                      schedule_log);
    }

    ruuvi_interface_timer_start (m_log_timer, APPLICATION_ENVIRONMENTAL_TICK_MS);
    // Log environmental conditions at start
    ruuvi_interface_scheduler_event_put (NULL, 0, execute_log);
    */
    return (NULL == m_active_sensor) ? RUUVI_DRIVER_ERROR_NOT_FOUND : RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_environmental_sample (void)
{
    if (NULL == m_active_sensor->mode_set)
    {
        return RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
    return m_active_sensor->mode_set (&mode);
}

ruuvi_driver_status_t task_environmental_data_get (ruuvi_driver_sensor_data_t * const
        p_data)
{
    if (NULL == p_data)
    {
        return RUUVI_DRIVER_ERROR_NULL;
    }

    if (NULL == m_active_sensor->data_get)
    {
        return RUUVI_DRIVER_ERROR_INVALID_STATE;
    }

    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = m_active_sensor->data_get (p_data);

    if (!p_data->valid.datas.pressure_pa)
    {
        ruuvi_driver_sensor_t * p_bme;
        p_bme = task_sensor_find_backend (m_environmental_sensors,
                                          ENV_SENSOR_COUNT, "BME280");

        if (p_bme != NULL)
        {
            // Do not overwrite exisiting valid data
            // p_data->fields.datas.humidity_rh = !p_data->valid.datas.humidity_rh;
            // p_data->fields.datas.temperature_c = !p_data->valid.datas.temperature_c;
            p_bme->data_get (p_data);
            p_data->valid.datas.pressure_pa = 1;
            //  p_data->fields.datas.humidity_rh = p_data->valid.datas.humidity_rh;
            //  p_data->fields.datas.temperature_c = p_data->valid.datas.temperature_c;
        }
    }

    return err_code;
}

ruuvi_driver_status_t task_environmental_log (void)
{
    ruuvi_driver_sensor_data_t data = {0};
    float values[3];
    data.data = values;
    data.fields.datas.temperature_c = 1;
    data.fields.datas.humidity_rh = 1;
    data.fields.datas.pressure_pa = 1;
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= task_environmental_data_get (&data);
    RUUVI_DRIVER_ERROR_CHECK (err_code, ~RUUVI_DRIVER_ERROR_FATAL);
    uint64_t sampletime = data.timestamp_ms / 1000;
    uint32_t logtime    = (uint32_t) sampletime;
    uint64_t systime = task_rtc_millis() / 1000;

    if (logtime > systime)
    {
        LOGW ("Stored sample in the future\r\n");
    }

    environmental_log_t log = { .timestamp_s   = logtime,
                                .temperature_c = ruuvi_driver_sensor_data_parse (&data, (ruuvi_driver_sensor_data_fields_t)
    {
        .datas.temperature_c = 1
    }),
    .humidity_rh   = ruuvi_driver_sensor_data_parse (&data, (ruuvi_driver_sensor_data_fields_t)
    {
        .datas.humidity_rh = 1
    }),
    .pressure_pa   = ruuvi_driver_sensor_data_parse (&data, (ruuvi_driver_sensor_data_fields_t)
    {
        .datas.pressure_pa = 1
    })
                              };
    ruuvi_library_status_t status = ruuvi_library_ringbuffer_queue (&ringbuf, &log,
                                    sizeof (log));

    // Drop old sample if buffer is full
    if (RUUVI_LIBRARY_ERROR_NO_MEM == status)
    {
        LOG ("Discarded data... ");
        environmental_log_t drop;
        status = ruuvi_library_ringbuffer_dequeue (&ringbuf, &drop);
        status |= ruuvi_library_ringbuffer_queue (&ringbuf, &log, sizeof (log));
    }

    RUUVI_DRIVER_ERROR_CHECK (status, ~RUUVI_DRIVER_ERROR_FATAL);

    if (RUUVI_DRIVER_SUCCESS == (status | err_code))
    {
        LOG ("Stored data\r\n");
    }

    return status | err_code;
}

ruuvi_driver_status_t task_environmental_log_read (const
        ruuvi_interface_communication_xfer_fp_t reply_fp,
        const ruuvi_interface_communication_message_t * const query)
{
    LOG ("Preparing to send logs\r\n");
    ruuvi_interface_communication_message_t msg = { 0 };
    uint64_t systime = task_rtc_millis() / 1000;
    uint32_t now = (query->data[3] << 24) +
                   (query->data[4] << 16) +
                   (query->data[5] << 8) +
                   (query->data[6] << 0);
    uint32_t start = (query->data[7] << 24) +
                     (query->data[8] << 16) +
                     (query->data[9] << 8) +
                     (query->data[10] << 0);
    uint32_t offset = (now > systime) ? now - systime : 0;
    environmental_log_t * p_log;
    msg.data_length = RUUVI_ENDPOINT_STANDARD_MESSAGE_LENGTH;
    msg.data[0] = query->data[1];
    msg.data[1] = query->data[0];
    msg.data[2] = RUUVI_ENDPOINT_STANDARD_LOG_VALUE_WRITE;
    ruuvi_library_status_t status = RUUVI_LIBRARY_SUCCESS;
    // As long as we have more elements
    size_t index = 0;

    do
    {
        status = ruuvi_library_ringbuffer_peek (&ringbuf, &p_log, index++);

        // Send logged element
        if (RUUVI_LIBRARY_SUCCESS == status)
        {
            // Calculate real time of event
            uint32_t timestamp = offset + p_log->timestamp_s;

            // Check if the event is in range to send. Continue if not
            if (0 == p_log->timestamp_s)
            {
                LOG ("WARNING: Empty element\r\n");
                continue;
            }

            if (now - offset < p_log->timestamp_s)
            {
                LOG ("WARNING: Element in future\r\n");
                continue;
            }

            if (now - offset - p_log->timestamp_s > (2 * (ringbuf.index_mask *
                    (APPLICATION_ENVIRONMENTAL_LOG_INTERVAL_MS / 1000))))
            {
                LOG ("WARNING: Element in distant past\r\n");
                continue;
            }

            if (start > timestamp)
            {
                continue;
            }

            msg.data[3] = timestamp >> 24;
            msg.data[4] = timestamp >> 16;
            msg.data[5] = timestamp >> 8;
            msg.data[6] = timestamp >> 0;
            uint8_t destination = query->data[0];

            // send temp, humi, pressure
            if (RUUVI_ENDPOINT_STANDARD_DESTINATION_ENVIRONMENTAL == destination ||
                    RUUVI_ENDPOINT_STANDARD_DESTINATION_TEMPERATURE   == destination)
            {
                int32_t temperature_cc = p_log->temperature_c * 100;
                msg.data[1] = RUUVI_ENDPOINT_STANDARD_DESTINATION_TEMPERATURE;
                msg.data[7] = temperature_cc >> 24;
                msg.data[8] = temperature_cc >> 16;
                msg.data[9] = temperature_cc >> 8;
                msg.data[10] = temperature_cc >> 0;

                // Repeat sending here
                while (RUUVI_LIBRARY_ERROR_NO_MEM == reply_fp (&msg))
                {
                    // Sleep
                    ruuvi_interface_yield();
                }
            }

            if (RUUVI_ENDPOINT_STANDARD_DESTINATION_ENVIRONMENTAL == destination ||
                    RUUVI_ENDPOINT_STANDARD_DESTINATION_HUMIDITY      == destination)
            {
                uint32_t humidity_crh = p_log->humidity_rh * 100;
                msg.data[1] = RUUVI_ENDPOINT_STANDARD_DESTINATION_HUMIDITY;
                msg.data[7]  = humidity_crh >> 24;
                msg.data[8]  = humidity_crh >> 16;
                msg.data[9]  = humidity_crh >> 8;
                msg.data[10] = humidity_crh >> 0;

                // Repeat sending here
                while (RUUVI_LIBRARY_ERROR_NO_MEM == reply_fp (&msg))
                {
                    // Sleep
                    ruuvi_interface_yield();
                }
            }

            if (RUUVI_ENDPOINT_STANDARD_DESTINATION_ENVIRONMENTAL == destination ||
                    RUUVI_ENDPOINT_STANDARD_DESTINATION_PRESSURE      == destination)
            {
                if (isnan (p_log->pressure_pa))
                {
                    continue;
                }

                uint32_t pressure_pa = p_log->pressure_pa;
                msg.data[1] = RUUVI_ENDPOINT_STANDARD_DESTINATION_PRESSURE;
                msg.data[7]  = pressure_pa >> 24;
                msg.data[8]  = pressure_pa >> 16;
                msg.data[9]  = pressure_pa >> 8;
                msg.data[10] = pressure_pa >> 0;

                // Repeat sending here
                while (RUUVI_LIBRARY_ERROR_NO_MEM == reply_fp (&msg))
                {
                    // Sleep
                    ruuvi_interface_yield();
                }
            }
        }
    } while (RUUVI_LIBRARY_SUCCESS == status);

    LOG ("Logs sent\r\n");
    return RUUVI_DRIVER_SUCCESS;
}

ruuvi_driver_status_t task_environmental_backend_set (const char * const name)
{
    ruuvi_driver_sensor_t * p_backend;
    p_backend = task_sensor_find_backend (m_environmental_sensors, ENV_SENSOR_COUNT, name);

    if (NULL != p_backend)
    {
        m_active_sensor = p_backend;
        return RUUVI_DRIVER_SUCCESS;
    }

    return RUUVI_DRIVER_ERROR_NOT_FOUND;
}