#include "unity.h"

#include "app_config.h"
#include "app_log.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_library.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_ruuvi_task_flash.h"
#include "mock_ruuvi_library_compress.h"

#include <string.h>

#define NUM_FIELDS 4 //XXX, should support any number of fields
#define IGNORE_RECORD_IDX 0xFFU

extern app_log_record_t    m_log_input_block;
extern app_log_record_t    m_log_output_block;
extern rl_compress_state_t m_compress_state;
extern app_log_config_t    m_log_config;
extern uint64_t            m_last_sample_ms;

void setUp (void)
{
    ri_log_Ignore();
    rd_error_check_Ignore();
}

void tearDown (void)
{
}

static void store_block_expect (const uint8_t record_idx, const bool block_flash)
{
    rt_flash_free_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx,
                                   RD_SUCCESS);

    if (IGNORE_RECORD_IDX == record_idx)
    {
        rt_flash_free_IgnoreArg_record_id();
    }

    rt_flash_busy_ExpectAndReturn (block_flash);

    if (block_flash)
    {
        ri_yield_ExpectAndReturn (RD_SUCCESS);
        rt_flash_busy_ExpectAndReturn (false);
    }

    rt_flash_gc_run_ExpectAndReturn (RD_SUCCESS);
    rt_flash_busy_ExpectAndReturn (block_flash);

    if (block_flash)
    {
        ri_yield_ExpectAndReturn (RD_SUCCESS);
        rt_flash_busy_ExpectAndReturn (false);
    }

    rt_flash_store_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                    (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx,
                                    &m_log_input_block, sizeof (m_log_input_block),
                                    RD_SUCCESS);

    if (IGNORE_RECORD_IDX == record_idx)
    {
        rt_flash_store_IgnoreArg_record_id();
    }

    rt_flash_busy_ExpectAndReturn (block_flash);

    if (block_flash)
    {
        ri_yield_ExpectAndReturn (RD_SUCCESS);
        rt_flash_busy_ExpectAndReturn (false);
    }
}

/**
 * @brief Initialize logging.
 *
 * After initialization driver is ready to store data with @ref app_log_process.
 * If there is a logging configuration stored to flash, stored configuration is used.
 * If not, default configuration is used and stored to flash.
 *
 * @retval RD_SUCCESS if logging was initialized.
 * @retval RD_ERROR_INVALID_STATE if flash task is not initialized.
 */
void test_app_log_init_nostored (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t defaults =
    {
        .interval_s = APP_LOG_INTERVAL_S,
        .overflow   = APP_LOG_OVERFLOW,
        .fields = {
            .datas.temperature_c = APP_LOG_TEMPERATURE_ENABLED,
            .datas.humidity_rh = APP_LOG_HUMIDITY_ENABLED,
            .datas.pressure_pa = APP_LOG_PRESSURE_ENABLED
        }
    };
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_CONFIG_RECORD,
                                   &defaults, sizeof (defaults),
                                   RD_ERROR_NOT_FOUND);
    rt_flash_load_IgnoreArg_message();
    rt_flash_store_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                    APP_FLASH_LOG_CONFIG_RECORD,
                                    &defaults, sizeof (defaults),
                                    RD_SUCCESS);
    rt_flash_store_IgnoreArg_message();
    err_code |= app_log_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&defaults, &m_log_config, sizeof (m_log_config)));
}

void test_app_log_init_stored (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t stored =
    {
        .interval_s = 10,
        .overflow   = false,
        .fields = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 0,
            .datas.pressure_pa = 1
        }
    };
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_CONFIG_RECORD,
                                   NULL, sizeof (stored),
                                   RD_SUCCESS);
    rt_flash_load_IgnoreArg_message();
    rt_flash_load_ReturnMemThruPtr_message (&stored, sizeof (stored));
    err_code |= app_log_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&stored, &m_log_config, sizeof (m_log_config)));
}

void test_app_log_init_noflash (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t defaults = {0};
    memcpy (&defaults, &m_log_config, sizeof (defaults));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_CONFIG_RECORD,
                                   &defaults, sizeof (defaults),
                                   RD_ERROR_INVALID_STATE);
    rt_flash_load_IgnoreArg_message();
    err_code |= app_log_init();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
    TEST_ASSERT (!memcmp (&defaults, &m_log_config, sizeof (m_log_config)));
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
 */
void test_app_log_process_ok (void)
{
    rd_status_t err_code = RD_SUCCESS;
    float samples[NUM_FIELDS] = {0};
    const rd_sensor_data_t sample =
    {
        .timestamp_ms = 1U,
        .fields = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 1,
            .datas.pressure_pa = 1,
            .datas.voltage_v = 1
        },
        .valid = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 1,
            .datas.pressure_pa = 1,
            .datas.voltage_v = 1
        },
        .data = samples
    };
    rl_compress_ExpectAndReturn (NULL,
                                 m_log_input_block.storage,
                                 sizeof (m_log_input_block.storage),
                                 &m_compress_state,
                                 RL_SUCCESS);
    rl_compress_IgnoreArg_data();
    err_code |= app_log_process (&sample);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_process_sequence (void)
{
    rd_status_t err_code = RD_SUCCESS;
    float samples[NUM_FIELDS] = {0};
    const uint16_t interval_s = 4U;
    m_last_sample_ms = 0;
    app_log_config_t store =
    {
        .interval_s = interval_s,
        .overflow   = APP_LOG_OVERFLOW,
        .fields = {
            .datas.temperature_c = APP_LOG_TEMPERATURE_ENABLED,
            .datas.humidity_rh = APP_LOG_HUMIDITY_ENABLED,
            .datas.pressure_pa = APP_LOG_PRESSURE_ENABLED
        }
    };
    memcpy (&m_log_config, &store, sizeof (m_log_config));
    rd_sensor_data_t sample =
    {
        .timestamp_ms = 1U,
        .fields = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 1,
            .datas.pressure_pa = 1,
            .datas.voltage_v = 1
        },
        .valid = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 1,
            .datas.pressure_pa = 1,
            .datas.voltage_v = 1
        },
        .data = samples
    };

    for (size_t ii = 0; ii < 6; ii++)
    {
        if (! (ii % 2))
        {
            rl_compress_ExpectAndReturn (NULL,
                                         m_log_input_block.storage,
                                         sizeof (m_log_input_block.storage),
                                         &m_compress_state,
                                         RL_SUCCESS);
            rl_compress_IgnoreArg_data();
        }

        err_code |= app_log_process (&sample);
        sample.timestamp_ms += (interval_s / 2) * 1000;
    }

    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_process_fill_blocks (void)
{
    rd_status_t err_code = RD_SUCCESS;
    m_last_sample_ms = 0;
    float samples[4] = {0}; //!< number of fields to mock-store.
    rd_sensor_data_t sample =
    {
        .timestamp_ms = 1U,
        .fields = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 1,
            .datas.pressure_pa = 1,
            .datas.voltage_v = 1
        },
        .valid = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 1,
            .datas.pressure_pa = 1,
            .datas.voltage_v = 1
        },
        .data = samples
    };
    uint8_t record_idx = 0;

    for (size_t ii = 0; ii < APP_FLASH_LOG_DATA_RECORDS_NUM * 2; ii++)
    {
        rl_compress_ExpectAndReturn (NULL,
                                     m_log_input_block.storage,
                                     sizeof (m_log_input_block.storage),
                                     &m_compress_state,
                                     RL_COMPRESS_END);
        rl_compress_IgnoreArg_data();
        store_block_expect (record_idx, ii % 2);
        sample.timestamp_ms += (m_log_config.interval_s * 1001U);
        record_idx++;
        record_idx = record_idx % APP_FLASH_LOG_DATA_RECORDS_NUM;
        err_code |= app_log_process (&sample);
    }

    TEST_ASSERT (RD_SUCCESS == err_code);
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
void test_app_log_read_from_start (void)
{
    rd_status_t err_code = RD_SUCCESS;
    float samples[NUM_FIELDS] = {0};
    rd_sensor_data_t sample =
    {
        .timestamp_ms = 2000U,
        .fields = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 1,
            .datas.pressure_pa = 1,
            .datas.voltage_v = 1
        },
        .valid = { 0 },
        .data = samples
    };
    app_log_record_t record = {0};
    record.start_timestamp_s = 1U;
    record.end_timestamp_s = 10U;
    rl_data_t data = {0};
    uint32_t timestamp = sample.timestamp_ms / 1000;
    uint8_t record_idx = 0;
    // Stash current block.
    store_block_expect (record_idx, false);
    // Load flash, check if we can find a block which has start timestamp before
    // and end timestamp after target time.
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx,
                                   &record, sizeof (record),
                                   RD_SUCCESS);
    rt_flash_load_IgnoreArg_message();
    rt_flash_load_ReturnArrayThruPtr_message (&record, 1);
    rl_decompress_ExpectWithArrayAndReturn (&data, 1,
                                            m_log_output_block.storage, sizeof (m_log_output_block.storage),
                                            sizeof (m_log_output_block.storage),
                                            &m_compress_state, 1,
                                            &timestamp, 1,
                                            RD_SUCCESS);
    err_code |= app_log_read (&sample);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

/**
 * @brief Configure logging.
 *
 * Calling this function will flush current log buffer into flash, possibly leading
 * to NULL entries.
 *
 * @retval RD_SUCCESS on successful configuration of log.
 * @retcal RD_ERROR_INVALID_STATE if flash is not initialized.
 */
void test_app_log_config_set_ok (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t defaults =
    {
        .interval_s = APP_LOG_INTERVAL_S,
        .overflow   = APP_LOG_OVERFLOW,
        .fields = {
            .datas.temperature_c = APP_LOG_TEMPERATURE_ENABLED,
            .datas.humidity_rh = APP_LOG_HUMIDITY_ENABLED,
            .datas.pressure_pa = APP_LOG_PRESSURE_ENABLED
        }
    };
    rt_flash_store_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                    APP_FLASH_LOG_CONFIG_RECORD,
                                    &defaults, sizeof (defaults),
                                    RD_SUCCESS);
    rt_flash_store_IgnoreArg_message();
    store_block_expect (IGNORE_RECORD_IDX, false);
    err_code |= app_log_config_set (&defaults);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_config_set_notinit (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t defaults =
    {
        .interval_s = APP_LOG_INTERVAL_S,
        .overflow   = APP_LOG_OVERFLOW,
        .fields = {
            .datas.temperature_c = APP_LOG_TEMPERATURE_ENABLED,
            .datas.humidity_rh = APP_LOG_HUMIDITY_ENABLED,
            .datas.pressure_pa = APP_LOG_PRESSURE_ENABLED
        }
    };
    rt_flash_store_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                    APP_FLASH_LOG_CONFIG_RECORD,
                                    &defaults, sizeof (defaults),
                                    RD_ERROR_INVALID_STATE);
    rt_flash_store_IgnoreArg_message();
    err_code |= app_log_config_set (&defaults);
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
}


/**
 * @brief Read current logging configuration.
 *
 * @retval RD_SUCCESS if configuration was loaded successfully.
 * @retval RD_ERROR_INVALID_STATE if flash is not initialized.
 * @retval RD_ERROR_NOT_FOUND if configuration could not found.
 */
void test_app_log_config_get_not_found (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t defaults =
    {
        .interval_s = APP_LOG_INTERVAL_S,
        .overflow   = APP_LOG_OVERFLOW,
        .fields = {
            .datas.temperature_c = APP_LOG_TEMPERATURE_ENABLED,
            .datas.humidity_rh = APP_LOG_HUMIDITY_ENABLED,
            .datas.pressure_pa = APP_LOG_PRESSURE_ENABLED
        }
    };
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_CONFIG_RECORD,
                                   &defaults, sizeof (defaults),
                                   RD_ERROR_NOT_FOUND);
    rt_flash_load_IgnoreArg_message();
    err_code |= app_log_config_get (&defaults);
    TEST_ASSERT (RD_ERROR_NOT_FOUND == err_code);
}

void test_app_log_config_get_found (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t defaults =
    {
        .interval_s = APP_LOG_INTERVAL_S,
        .overflow   = APP_LOG_OVERFLOW,
        .fields = {
            .datas.temperature_c = APP_LOG_TEMPERATURE_ENABLED,
            .datas.humidity_rh = APP_LOG_HUMIDITY_ENABLED,
            .datas.pressure_pa = APP_LOG_PRESSURE_ENABLED
        }
    };
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_CONFIG_RECORD,
                                   &defaults, sizeof (defaults),
                                   RD_SUCCESS);
    rt_flash_load_IgnoreArg_message();
    err_code |= app_log_config_get (&defaults);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_config_get_not_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t defaults =
    {
        .interval_s = APP_LOG_INTERVAL_S,
        .overflow   = APP_LOG_OVERFLOW,
        .fields = {
            .datas.temperature_c = APP_LOG_TEMPERATURE_ENABLED,
            .datas.humidity_rh = APP_LOG_HUMIDITY_ENABLED,
            .datas.pressure_pa = APP_LOG_PRESSURE_ENABLED
        }
    };
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_CONFIG_RECORD,
                                   &defaults, sizeof (defaults),
                                   RD_ERROR_NOT_INITIALIZED);
    rt_flash_load_IgnoreArg_message();
    err_code |= app_log_config_get (&defaults);
    TEST_ASSERT (RD_ERROR_NOT_INITIALIZED == err_code);
}

/** @} */
