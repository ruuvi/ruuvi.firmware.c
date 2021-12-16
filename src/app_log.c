/**
 * @file app_log.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-07-17
 *       2021-12-07 remove unused app_log_config_get/set,
 *       2021-05-06 includes Potential fix for #255 (#256)
 * @brief
 * Save and retrieve sensor readings to/from flash
 *  for transmitting to station for sync.
 * @detail
 * Allocate static input, output and config memory.
 * Readings are blocked into a nearly page size buffer.
 * store_block by bumping index up to _DATA_RECORDS_NUM then wrap.
 * @note>> As of 2021-12-14, 3.31.1 .overflow is not checked
 *   Since recordKey wraps flash will never fillup.
 *   There is no mechanism to retain old data and stop saving samples.
 *
 * _log_process establish time for next sample
 *              if it's time: collect sensor data and add to input_block and
 *              if full: write it to flash
 * _init called by setup: config_set/get to/from flash. and boot_count and  (( Why is boot_count here?)
 *          _purge_logs erases all log records then garbage collection.
 * Use FDS record access routines (not fstore page routines)
 *  via rt_flash_free, _store, _load and _flash_gc_run
 * Possible fatal error from FDS: delete SPACE_IN_QUEUES,
 * https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.0.0/lib_fds_functionality.html
 *
 * (_) ToDo Save RAM by fetchihng sync data directly from flash and elimitating input_block buffer.
 *
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */
#include "app_log.h"

#include "app_config.h"
#include "app_testing.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_library.h"
#include "ruuvi_library_compress.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_flash.h"

#if RT_FLASH_ENABLED

#ifndef LOGI
static inline void LOGI (const char * const msg) { ri_log (RI_LOG_LEVEL_INFO,  msg); }
static inline void LOGD (const char * const msg) { ri_log (RI_LOG_LEVEL_DEBUG, msg); }
static inline void LOGE (const char * const msg) { ri_log (RI_LOG_LEVEL_ERROR, msg); }
#endif

#define YIELD_TIL_DONE       while (rt_flash_busy()) { ri_yield(); }  // astyle work around

/**
 * @addtogroup app_log
 */
/** @{ */

TESTABLE_STATIC app_log_record_t m_log_input_block;  //!< to be stored to flash.
TESTABLE_STATIC app_log_record_t m_log_output_block; //!< read from flash for examination.

TESTABLE_STATIC app_log_config_t m_log_config;     //!< Configuration for logging.
TESTABLE_STATIC uint64_t         m_last_sample_ms; //!< Timestamp of last processed sample
TESTABLE_STATIC uint32_t         m_boot_count = 0;
                bool             m_log_config_retain = APP_FLASH_LOG_CONFIG_NVM_ENABLED;

/* @brief Start at next block number from where we left off.
 *          Free, GC, Store */        /* this code is kind "hinky" ?? DG */
// // // // // // /           // // // // // // // // // //
static rd_status_t store_block (const app_log_record_t * const p_record)
{
    rd_status_t err_code = RD_SUCCESS;
    uint8_t num_tries = 0;
    static uint8_t record_idx = 0;

    do
    {
        uint8_t  record_slot = (record_idx + num_tries) %
                               APP_FLASH_LOG_DATA_RECORDS_NUM; // if while went past the end, wrap
        uint16_t target_record = (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8u) + record_slot;
        err_code = rt_flash_free (APP_FLASH_LOG_FILE, target_record); // free old one to GC
        char msg[128];

        if (RD_SUCCESS == err_code) // It's not a problem if there wasn't old block to erase.
        {
            snprintf (msg, sizeof (msg), "store_block:freed old record #%04X\n", target_record);
            LOGI (msg);
        }
        else
        {
            snprintf (msg, sizeof (msg), "store_block:creating new record #%04X\n", target_record);
            LOGI (msg);
        }

        err_code &= ~RD_ERROR_NOT_FOUND;    // clear NOT_FOUND Shouldn't we wait until it done ??
        YIELD_TIL_DONE;                                                     //      and wait
        //
        err_code |= rt_flash_gc_run ();                                      // GC and wait
        YIELD_TIL_DONE;
        //
        err_code |= rt_flash_store (APP_FLASH_LOG_FILE, target_record,       // store it and wait
                                    p_record, sizeof (app_log_record_t));
        YIELD_TIL_DONE;
        RD_ERROR_CHECK (err_code, ~RD_ERROR_FATAL);
        num_tries++;             // Try the next block if there was error.
    } while ( (RD_SUCCESS != err_code) && (num_tries < APP_FLASH_LOG_DATA_RECORDS_NUM));

    if (RD_SUCCESS == err_code)
    {
        record_idx += num_tries;         // next time start where we left off
        record_idx = record_idx % APP_FLASH_LOG_DATA_RECORDS_NUM;         // wrap back to zero
    }

    return err_code;
}
// // // // // // /           // // // // // // // // // //
static rd_status_t purge_logs (void)
{
    rd_status_t err_code = RD_SUCCESS;
    LOGI ("Free all & GC\n");

    for (uint8_t record_idx = 0; record_idx < APP_FLASH_LOG_DATA_RECORDS_NUM; record_idx++)
    {
        err_code |= rt_flash_free (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8u) + record_idx);
        YIELD_TIL_DONE;
    }

    err_code &= ~RD_ERROR_NOT_FOUND; // It doesn't matter if there was no data to erase.
    err_code |= rt_flash_gc_run ();
    return err_code;
}
// // // // // // // // // // // //                // // //
TESTABLE_STATIC rd_status_t app_log_read_boot_count (
    void)   // should be called load, bump, store
{
    rd_status_t err_code = RD_SUCCESS;
    err_code |= rt_flash_load (APP_FLASH_LOG_FILE,
                               APP_FLASH_LOG_BOOT_COUNTER_RECORD,           // load
                               &m_boot_count, sizeof (m_boot_count));

    if ( (RD_SUCCESS == err_code) || (RD_ERROR_NOT_FOUND == err_code))
    {
        m_boot_count++;
        err_code = rt_flash_store (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_BOOT_COUNTER_RECORD,       // bump and store
                                   &m_boot_count, sizeof (m_boot_count));
    }

    char msg[128];
    snprintf (msg, sizeof (msg), "Boot count: %ld\n", m_boot_count);
    LOGI (msg);
    return err_code;
}

// // // // // /         / // // // // // // // // // // //
/* @brief _init : load config (stores defaults the first time)
 *        purge_logs
 *        _read_boot_count LOGs it too
 *           */
rd_status_t app_log_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    // Defaults may get overwritten by flash load
    app_log_config_t config =
    {
        .interval_s = APP_LOG_INTERVAL_S,
        .overflow   = APP_LOG_OVERFLOW,   // ineffective as of 2021-12-14 v3.31.1
        .fields = {
            .datas.temperature_c = APP_LOG_TEMPERATURE_ENABLED,
            .datas.humidity_rh   = APP_LOG_HUMIDITY_ENABLED,
            .datas.pressure_pa   = APP_LOG_PRESSURE_ENABLED
        }
    };
    if ( m_log_config_retain )
        { err_code = rt_flash_load  (APP_FLASH_LOG_FILE, APP_FLASH_LOG_CONFIG_RECORD,   // load prior config
                                     &config, sizeof (config));
        }
    if (RD_ERROR_NOT_FOUND == err_code)
        { err_code = rt_flash_store (APP_FLASH_LOG_FILE, APP_FLASH_LOG_CONFIG_RECORD,   // store config (on first time)
                                     &config, sizeof (config));
        }
    if (RD_SUCCESS == err_code)
    {
        memcpy (&m_log_config, &config, sizeof (config));       // copy local buffer to static Why not just read into m_ ??

/*      Maybe user wants to keep current ( albiet stale) history.
 *      Example: temperature and battery status cause power "blip".
 *      Timestamps on data in flash for NEW entries will be  less than elements already on history log.
 *      Leave it up to  the user */
        LOGI ("purge\n");
        err_code |= purge_logs();
    }

    err_code |= app_log_read_boot_count();
    return err_code;
}
/* @brief Process data into log.
 *
 * @detail If time elapsed since last logged element is larger than logging interval,
 * data is stored to RAM buffer.
 * When the buffer fills
 * @ref APP_LOG_MAX_SAMPLES it will be written to flash. */

// // // // // //             // // // // // // // // // //
rd_status_t app_log_process (const rd_sensor_data_t * const sample)
{
    rd_status_t err_code = RD_SUCCESS;
    uint64_t next_sample_ms = m_last_sample_ms + (m_log_config.interval_s * 1000u);
    uint32_t end_timestamp = m_log_input_block.end_timestamp_s;
    LOGD ("Sample received  ");

    if (0 == m_last_sample_ms) { next_sample_ms = 0; } // Always store first sample.

    if (next_sample_ms <= sample->timestamp_ms) //Check if new sample should be processed
    {
        LOGD ("Storing sample\n");
        app_log_element_t element =
        {
            .timestamp_s   = sample->timestamp_ms / 1000u,
            .temperature_c = rd_sensor_data_parse (sample, RD_SENSOR_TEMP_FIELD),
            .humidity_rh   = rd_sensor_data_parse (sample, RD_SENSOR_HUMI_FIELD),
            .pressure_pa   = rd_sensor_data_parse (sample, RD_SENSOR_PRES_FIELD),
        };
        // .h                                           STORAGE_RECORD_HEADER_SIZE 96 (with spare)
        // .h  STORAGE_BLOCK_SIZE (RB_FLASH_PAGE_SIZE - STORAGE_RECORD_HEADER_SIZE)
        // .h:_MAX_SAMPLES (STORAGE_BLOCK_SIZE/sizeof(app_log_element_t))

        if (m_log_input_block.num_samples < APP_LOG_MAX_SAMPLES)
        {   m_log_input_block.storage[m_log_input_block.num_samples++] = element; }  // embedded ++
        /**/
        else
        {
            err_code |= store_block (&m_log_input_block);
            RD_ERROR_CHECK (err_code, RD_SUCCESS);
            memset (&m_log_input_block, 0, sizeof (m_log_input_block));   // zero input_block WHY??
            m_log_input_block.start_timestamp_s = end_timestamp;
        }

        m_last_sample_ms = sample->timestamp_ms;
        m_log_input_block.end_timestamp_s = sample->timestamp_ms / 1000u;
    }
    return err_code;
}

/**
 * @brief Load new block to be read if needed.
 *
 * Can also copy input block to
 * output block if there's no more stored blocks in flash.
 */
// // // // // // // // // /              // // // // // //
static rd_status_t app_log_read_load_block (app_log_read_state_t * const p_rs)
{
    rd_status_t err_code = RD_SUCCESS;

    if ( (0 == p_rs->element_idx)
            && (0 == p_rs->page_idx))
    {
        // Clear out previous state
        err_code |= rt_flash_load (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8u) + p_rs->page_idx,
                                   &m_log_output_block, sizeof (m_log_output_block));
        p_rs->page_idx++;
    }
    else if ( (APP_FLASH_LOG_DATA_RECORDS_NUM > p_rs->page_idx)
              && (p_rs->element_idx >= m_log_output_block.num_samples))
    {
        // Returns NOT_FOUND if page IDX is not in flash.
        err_code |= rt_flash_load (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8u) + p_rs->page_idx,
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
    else {} // No action needed.

    // Zero out state if block was not found
    if (RD_ERROR_NOT_FOUND == err_code)
        { memset (&m_log_output_block, 0, sizeof (m_log_output_block)); }

    return err_code;
}

/**
 * @brief Forward read state to first next valid element.
 *  internal
 * @retval RD_SUCCESS p_rs points to a valid element
 * @retval RD_ERROR_NOT_FOUND if block doesn't have a valid element.
 */
// // // // // // // // //                   // // // // //
static rd_status_t app_log_read_fast_forward (app_log_read_state_t * const p_rs)
{
    rd_status_t err_code = RD_SUCCESS;
    uint64_t ts_s  = m_log_output_block.storage[p_rs->element_idx].timestamp_s;
    uint64_t ts_ms = ts_s * 1000LLU;

    while ( (p_rs->oldest_element_ms > ts_ms)
            && (p_rs->element_idx < m_log_output_block.num_samples))
    {
        ts_s  = m_log_output_block.storage[p_rs->element_idx].timestamp_s;
        ts_ms = ts_s * 1000LLU;
        p_rs->element_idx++;
    }

    if (p_rs->element_idx >= m_log_output_block.num_samples) { err_code |= RD_ERROR_NOT_FOUND; }

    return err_code;
}
// // // // // // // // //                // // // // // //
static rd_status_t app_log_read_populate (rd_sensor_data_t * const sample,
        app_log_read_state_t * const p_rs)
{
    rd_status_t err_code = RD_SUCCESS;

    // Last memory block is the current RAM buffer, so we have valid data
    // when APP_FLASH_LOG_DATA_RECORDS_NUM + 1 == p_rs->page_idx
    if ( (APP_FLASH_LOG_DATA_RECORDS_NUM + 1) < p_rs->page_idx)
    {
        err_code |= RD_ERROR_NOT_FOUND;
    }
    else if (p_rs->element_idx < m_log_output_block.num_samples)
    {
        const app_log_element_t * const p_el = &m_log_output_block.storage[p_rs->element_idx];
        rd_sensor_data_set (sample, RD_SENSOR_TEMP_FIELD, p_el->temperature_c);
        rd_sensor_data_set (sample, RD_SENSOR_HUMI_FIELD, p_el->humidity_rh);
        rd_sensor_data_set (sample, RD_SENSOR_PRES_FIELD, p_el->pressure_pa);
        sample->timestamp_ms = ( (uint64_t) (p_el->timestamp_s)) * 1000LLU;
        p_rs->element_idx++;
    }
    else {} // No action required.

    return err_code;
}
// // // // // // //       // // // // // // // // // // //
rd_status_t app_log_read (rd_sensor_data_t * const sample,
                          app_log_read_state_t * const p_rs)
{
    rd_status_t err_code = RD_SUCCESS;

    if ( (NULL != sample) && (NULL != p_rs))
    {
        // Load new block if needed
        do
        {
            err_code = app_log_read_load_block (p_rs);

            // Check if ths block contains data in desired time range - TODO
            // Fast forward to start of desired time range.
            if (RD_SUCCESS == err_code) { err_code |= app_log_read_fast_forward (p_rs); }
        } while ( (err_code != RD_SUCCESS)

                  && (p_rs->page_idx <= APP_FLASH_LOG_DATA_RECORDS_NUM));

        err_code |= app_log_read_populate (sample, p_rs); // Populate record
    }
    else { err_code = RD_ERROR_NULL; }

    return err_code;
}
/* @brief user request system reset via button  */
// // // // /            / // // // // // // // // // // //
void app_log_purge_flash (void)
{
    LOGE("--\n");  // LOG will remote function name app_log_putg_flash. Enough said!
    ri_flash_purge();
}

#else     // RT_FLASH_ENABLED   //  //  //  //  //  //  //  //  //  //
rd_status_t app_log_init (void)                                                 // dummy
{
    return RD_SUCCESS;
}
rd_status_t app_log_process (const rd_sensor_data_t * const sample)             // dummy
{
    return RD_SUCCESS;
}
rd_status_t app_log_config_get (app_log_config_t * const configuration)         // dummy
{
    return RD_SUCCESS;
}
rd_status_t app_log_config_set (const app_log_config_t * const configuration)  // dummy
{
    return RD_SUCCESS;
}
rd_status_t app_log_read (rd_sensor_data_t * const sample,                     // dummy
                          app_log_read_state_t * const p_read_state)
{
    return RD_ERROR_NOT_FOUND;
}

void app_log_purge_flash (void)                                                 // dummy
{
    return;
}
#endif    // RT_FLASH_ENABLED
