#include "app_log.h"

#include "app_config.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_library.h"
#include "ruuvi_library_compress.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_flash.h"
#if RT_FLASH_ENABLED

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

#ifndef CEEDLING
static
#endif
app_log_config_t m_log_config; //!< Configuration for logging.

#ifndef CEEDLING
static
#endif
uint64_t m_last_sample_ms; //!< Timestamp of last processed sample.

static rd_status_t store_block (const app_log_record_t * const record)
{
    static uint8_t record_idx = 0;
    uint8_t num_tries = 0;
    uint32_t end_timestamp = m_log_input_block.end_timestamp_s;
    rd_status_t err_code = RD_SUCCESS;

    do
    {
        err_code = rt_flash_free (APP_FLASH_LOG_FILE,
                                  (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx);

        // It's not a problem if there wasn't old block to erase.
        if (RD_SUCCESS == err_code)
        {
            LOG ("LOG: Erasing old block\r\n");
        }

        err_code &= ~RD_ERROR_NOT_FOUND;

        while (rt_flash_busy())
        {
            ri_yield();
        }

        err_code |= rt_flash_gc_run ();

        while (rt_flash_busy())
        {
            ri_yield();
        }

        err_code |= rt_flash_store (APP_FLASH_LOG_FILE,
                                    (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx,
                                    &m_log_input_block, sizeof (m_log_input_block));

        while (rt_flash_busy())
        {
            ri_yield();
        }

        RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
        // Erase another block and try again if there was error.
        num_tries++;
        record_idx++;
        record_idx = record_idx % APP_FLASH_LOG_DATA_RECORDS_NUM;
    } while ( (RD_SUCCESS != err_code) && (num_tries < APP_FLASH_LOG_DATA_RECORDS_NUM));

    memset (&m_log_input_block, 0, sizeof (m_log_input_block));
    m_log_input_block.start_timestamp_s = end_timestamp;
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

rd_status_t app_log_process (const rd_sensor_data_t * const sample)
{
    rd_status_t err_code = RD_SUCCESS;
    uint64_t next_sample_ms = m_last_sample_ms + (m_log_config.interval_s * 1000U);
    LOGD ("LOG: Sample received\r\n");

    // Always store first sample.
    if (0 == m_last_sample_ms)
    {
        next_sample_ms = 0;
    }

    //Check if new sample should be processed
    if (next_sample_ms <= sample->timestamp_ms)
    {
        LOGD ("LOG: Storing sample\r\n");
        app_log_element_t element =
        {
            .timestamp_s = sample->timestamp_ms / 1000U,
            .temperature_c = rd_sensor_data_parse (sample, RD_SENSOR_TEMP_FIELD),
            .humidity_rh = rd_sensor_data_parse (sample, RD_SENSOR_HUMI_FIELD),
            .pressure_pa = rd_sensor_data_parse (sample, RD_SENSOR_PRES_FIELD),
        };

        if (APP_LOG_MAX_SAMPLES > m_log_input_block.num_samples)
        {
            m_log_input_block.storage[m_log_input_block.num_samples++] = element;
        }

        if (m_log_input_block.num_samples >= APP_LOG_MAX_SAMPLES)
        {
            LOG ("LOG: Storing block\r\n");
            err_code |= store_block (&m_log_input_block);
            RD_ERROR_CHECK (err_code, RD_SUCCESS);
        }

        m_last_sample_ms = sample->timestamp_ms;
        m_log_input_block.end_timestamp_s = sample->timestamp_ms / 1000U;
    }
    else
    {
        // No action needed.
    }

    return err_code;
}

/**
 * @brief Load new block to be read if needed.
 *
 * Can also copy input block to
 * output block if there's no more stored blocks in flash.
 */
static rd_status_t app_log_read_load_block (app_log_read_state_t * const p_rs)
{
    rd_status_t err_code = RD_SUCCESS;

    if ( (0 == p_rs->element_idx)
            && (0 == p_rs->page_idx))
    {
        // Clear out previous state
        err_code |= rt_flash_load (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + p_rs->page_idx,
                                   &m_log_output_block, sizeof (m_log_output_block));
        p_rs->page_idx++;
    }
    else if ( (APP_FLASH_LOG_DATA_RECORDS_NUM > p_rs->page_idx)
              && (p_rs->element_idx >= m_log_output_block.num_samples))
    {
        // Returns NOT_FOUND if page IDX is not in flash.
        err_code |= rt_flash_load (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + p_rs->page_idx,
                                   &m_log_output_block, sizeof (m_log_output_block));
        p_rs->page_idx++;
        p_rs->element_idx = 0;
    }
    else if ( (APP_FLASH_LOG_DATA_RECORDS_NUM == p_rs->page_idx)
              && (p_rs->element_idx >= m_log_output_block.num_samples))
    {
        memcpy (&m_log_output_block, &m_log_input_block, sizeof (m_log_output_block));
        p_rs->page_idx++;
        p_rs->element_idx = 0;
    }
    else
    {
        // No action needed.
    }

    // Clear out state if block was not found
    if (RD_ERROR_NOT_FOUND == err_code)
    {
        memset (&m_log_output_block, 0, sizeof (m_log_output_block));
    }

    return err_code;
}

/**
 * @brief Forward read state to first next valid element.
 *
 * @retval RD_SUCCESS p_rs points to a valid element
 * @retval RD_ERROR_NOT_FOUND if block doesn't have a valid element.
 */
static rd_status_t app_log_read_fast_forward (app_log_read_state_t * const p_rs)
{
    rd_status_t err_code = RD_SUCCESS;
    uint64_t ts = m_log_output_block.storage[p_rs->element_idx].timestamp_s * 1000U;

    while ( (p_rs->oldest_element_ms > ts)
            && (p_rs->element_idx < m_log_output_block.num_samples))
    {
        p_rs->element_idx++;
    }

    if (p_rs->element_idx >= m_log_output_block.num_samples)
    {
        err_code |= RD_ERROR_NOT_FOUND;
    }

    return err_code;
}

static rd_status_t app_log_read_populate (rd_sensor_data_t * const sample,
        app_log_read_state_t * const p_rs)
{
    rd_status_t err_code = RD_SUCCESS;

    if (p_rs->element_idx < m_log_output_block.num_samples)
    {
        const app_log_element_t * const p_el = &m_log_output_block.storage[p_rs->element_idx];
        rd_sensor_data_set (sample, RD_SENSOR_TEMP_FIELD, p_el->temperature_c);
        rd_sensor_data_set (sample, RD_SENSOR_HUMI_FIELD, p_el->humidity_rh);
        rd_sensor_data_set (sample, RD_SENSOR_PRES_FIELD, p_el->pressure_pa);
        sample->timestamp_ms = (uint64_t) (p_el->timestamp_s) * 1000U;
        p_rs->element_idx++;
    }
    else if (p_rs->page_idx >= APP_FLASH_LOG_DATA_RECORDS_NUM)
    {
        err_code |= RD_ERROR_NOT_FOUND;
    }
    else
    {
        // No action required.
    }

    return err_code;
}

rd_status_t app_log_read (rd_sensor_data_t * const sample,
                          app_log_read_state_t * const p_rs)
{
    rd_status_t err_code = RD_SUCCESS;

    if ( (NULL != sample)
            && (NULL != p_rs))
    {
        // Load new block if needed
        do
        {
            err_code = app_log_read_load_block (p_rs);

            // Decompress block -todo.
            // Check if ths block contains data in desired time range - TODO
            // Fast forward to start of desired time range.
            if (RD_SUCCESS == err_code)
            {
                err_code |= app_log_read_fast_forward (p_rs);
            }
        } while ( (err_code != RD_SUCCESS)

                  && (p_rs->page_idx <= APP_FLASH_LOG_DATA_RECORDS_NUM));

        // AStyle fails here. ^
        // Populate record
        err_code |= app_log_read_populate (sample, p_rs);
    }
    else
    {
        err_code = RD_ERROR_NULL;
    }

    return err_code;
}

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

        if (RD_SUCCESS == err_code)
        {
            store_block (&m_log_input_block);
            memcpy (&m_log_config, configuration, sizeof (m_log_config));
        }
    }

    return err_code;
}

rd_status_t app_log_config_get (app_log_config_t * const configuration)
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_flash_load (APP_FLASH_LOG_FILE,
                               APP_FLASH_LOG_CONFIG_RECORD,
                               configuration, sizeof (configuration));
    memcpy (configuration, &m_log_config, sizeof (m_log_config));
    return err_code;
}

void app_log_purge_flash (void)
{
    ri_flash_purge();
}

#else
//Dummy implementation to save flash space
rd_status_t app_log_init (void)
{
    return RD_SUCCESS;
}
rd_status_t app_log_process (const rd_sensor_data_t * const sample)
{
    return RD_SUCCESS;
}
rd_status_t app_log_config_get (app_log_config_t * const configuration)
{
    return RD_SUCCESS;
}
rd_status_t app_log_config_set (const app_log_config_t * const configuration)
{
    return RD_SUCCESS;
}
rd_status_t app_log_read (rd_sensor_data_t * const sample,
                          app_log_read_state_t * const p_read_state)
{
    return RD_ERROR_NOT_FOUND;
}

void app_log_purge_flash (void)
{
    return;
}
#endif

/** @} */
