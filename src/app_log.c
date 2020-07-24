#include "app_log.h"

#include "app_config.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_library.h"
#include "ruuvi_library_compress.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_flash.h"

static inline void LOG (const char * const msg)
{
    ri_log (RI_LOG_LEVEL_INFO, msg);
}

static inline void LOGD (const char * const msg)
{
    ri_log (RI_LOG_LEVEL_DEBUG, msg);
}


/**
 * @addtogroup app_log
 */
/** @{ */
/**
 * @file app_log.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-07-17
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef CEEDLING
static
#endif
app_log_record_t m_log_input_block;   //!< Block to be stored to flash.

#ifndef CEEDLING
static
#endif
app_log_record_t m_log_output_block;   //!< Block read from flash for examination.

#if ((!defined(CEEDLING)) && (!defined(RUUVI_RUN_TESTS)))
static
#endif
rl_compress_state_t m_compress_state; //!< State of compression library.

#ifndef CEEDLING
static
#endif
app_log_config_t m_log_config; //!< Configuration for logging.

/**
 * @brief Convert Ruuvi driver data to ruuvi library data.
 */
static rl_data_t rd_2_rl (const rd_sensor_data_t * const sample)
{
    const rl_data_t rl_data =
    {
        .time = (uint32_t) (sample->timestamp_ms / 1000U),
        .payload = {
            rd_sensor_data_parse (sample, RD_SENSOR_HUMI_FIELD),
            rd_sensor_data_parse (sample, RD_SENSOR_TEMP_FIELD),
            rd_sensor_data_parse (sample, RD_SENSOR_PRES_FIELD)
        }
    };
    return rl_data;
}

static rd_status_t store_block (const app_log_record_t * const record)
{
    static uint8_t record_idx = 0;
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_flash_free (APP_FLASH_LOG_FILE,
                               (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx);
    // It's not a problem if there wasn't old block to erase.
    err_code &= ~RD_ERROR_NOT_FOUND;
    while(rt_flash_busy())
    {
      ri_yield();
    }
    err_code |= rt_flash_gc_run ();
    while(rt_flash_busy())
    {
      ri_yield();
    }
    err_code |= rt_flash_store (APP_FLASH_LOG_FILE,
                                (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx,
                                &m_log_input_block, sizeof (m_log_input_block));
    while(rt_flash_busy())
    {
      ri_yield();
    }
    record_idx++;
    record_idx = record_idx % APP_FLASH_LOG_DATA_RECORDS_NUM;
    return err_code;
}

rd_status_t app_log_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    // Defaults get overwritten by flash load and stored if the load fails.
    app_log_config_t config =
    {
        .interval_s = APP_LOG_INTERVAL_S,
        .overflow   = APP_LOG_OVERFLOW,
        .fields = {
            .datas.temperature_c = APP_LOG_TEMPERATURE_ENABLED,
            .datas.humidity_rh = APP_LOG_HUMIDITY_ENABLED,
            .datas.pressure_pa = APP_LOG_PRESSURE_ENABLED
        }
    };
    err_code |= rt_flash_load (APP_FLASH_LOG_FILE,
                               APP_FLASH_LOG_CONFIG_RECORD,
                               &config, sizeof (config));

    if (RD_ERROR_NOT_FOUND == err_code)
    {
        err_code = rt_flash_store (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_CONFIG_RECORD,
                                   &config, sizeof (config));
    }

    if (RD_SUCCESS == err_code)
    {
        memcpy (&m_log_config, &config, sizeof (config));
    }

    return err_code;
}

/**
 * @brief Process data into log.
 *
 * If time elapsed since last logged element is larger than logging interval, data is
 * stored to RAM buffer. While data is being stored to RAM buffer, it is also stored to
 * compressed buffer. When compressed buffer fills 4000 bytes it will be written to flash.
 * If there is no more room for new blocks in flash, oldest flash block is erased and
 * replaced with new data.
 *
 * @retval RD_SUCCESS if data was logged.
 * @retval RD_ERROR_NO_MEMORY if data cannot be stored to flash and overflow is false.
 * @retval RD_ERROR_BUSY previous operation is in process, e.g. writing to flash.
 * @note   First sample is always stored.
 */
rd_status_t app_log_process (const rd_sensor_data_t * const sample)
{
    rd_status_t err_code = RD_SUCCESS;
    static uint64_t last_sample_ms;
    uint64_t next_sample_ms = last_sample_ms + (m_log_config.interval_s * 1000U);
    LOGD("LOG: Sample received\r\n");

    // Always store first sample.
    if (0 == last_sample_ms)
    {
        next_sample_ms = 0;
    }

    //Check if new sample should be processed
    if (next_sample_ms < sample->timestamp_ms)
    {
        LOGD("LOG: Storing sample\r\n");
        rl_status_t lib_status = RL_SUCCESS;
        rl_data_t data = rd_2_rl (sample);
        
        lib_status |= rl_compress (&data,
                                   m_log_input_block.storage,
                                   sizeof (m_log_input_block.storage),
                                   &m_compress_state);

        if (RL_SUCCESS == lib_status)
        {
            m_log_input_block.num_samples++;
        }
        else if (RL_COMPRESS_END == lib_status)
        {
            LOG("LOG: Storing block\r\n");
            m_log_input_block.num_samples++;
            m_log_input_block.end_timestamp_s = sample->timestamp_ms;
            err_code |= store_block (&m_log_input_block);
            memset (&m_compress_state, 0, sizeof (m_compress_state));
            memset (&m_log_input_block, 0, sizeof (m_log_input_block));
            m_log_input_block.start_timestamp_s = sample->timestamp_ms;
        }
        else
        {
            err_code |= RD_ERROR_INTERNAL;
            RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
        }

        last_sample_ms = sample->timestamp_ms;
    }

    return err_code;
}

/**
 * @brief Get data from log.
 *
 * Searches for first logged sample after given timestamp and returns it. Loop over
 * this function to get all logged data.
 *
 * @param[in,out] sample Input: Requested data fields with timestamp set.
 *                       Output: Filled fields with valid data.
 *
 * @retval RD_SUCCESS if a sample was retrieved.
 * @retval RD_ERROR_NOT_FOUND if no newer data than requested timestamp was found.
 *
 */
rd_status_t app_log_read (rd_sensor_data_t * const sample)
{
    return RD_ERROR_NOT_IMPLEMENTED;
}

/**
 * @brief Configure logging.
 *
 * Calling this function will flush current log buffer into flash, possibly leading
 * to NULL entries.
 */
rd_status_t app_log_config_set (const app_log_config_t * const configuration)
{
    rd_status_t err_code = RD_SUCCESS;

    if (NULL == configuration)
    {
        err_code |= RD_ERROR_NULL;
    }
    else
    {
        err_code |= rt_flash_store (APP_FLASH_LOG_FILE,
                                    APP_FLASH_LOG_CONFIG_RECORD,
                                    &configuration, sizeof (configuration));

        // TODO: Flush current data to flash
        if (RD_SUCCESS == err_code)
        {
            memcpy (&m_log_config, configuration, sizeof (m_log_config));
        }
    }

    return err_code;
}

/**
 * @brief Read current logging configuration.
 */
rd_status_t app_log_config_get (app_log_config_t * const configuration)
{
    rd_status_t err_code = RD_SUCCESS;
    memcpy(configuration, &m_log_config, sizeof(m_log_config));
    return err_code;
}

/** @} */
