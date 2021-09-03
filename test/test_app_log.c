#include "unity.h"

#include "app_config.h"
#include "app_log.h"
#include "ruuvi_library.h"
#include "ruuvi_interface_communication.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_driver_sensor.h"
#include "mock_ruuvi_endpoints.h"
#include "mock_ruuvi_interface_flash.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_rtc.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_ruuvi_task_flash.h"
#include "mock_ruuvi_library_compress.h"

#include <string.h>

#define NUM_FIELDS 4 //XXX, should support any number of fields
#define STORED_FIELDS  ( APP_LOG_TEMPERATURE_ENABLED + \
                         APP_LOG_HUMIDITY_ENABLED + \
                         APP_LOG_PRESSURE_ENABLED)
#define IGNORE_RECORD_IDX 0xFFU

const app_log_element_t e_1_1 =
{
    .timestamp_s = 800 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_1_2 =
{
    .timestamp_s = 1000 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_1_3 =
{
    .timestamp_s = 1200 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_1_4 =
{
    .timestamp_s = 1400 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_2_1 =
{
    .timestamp_s = 1600 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_2_2 =
{
    .timestamp_s = 1800 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_2_3 =
{
    .timestamp_s = 2000 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_2_4 =
{
    .timestamp_s = 2200 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_3_1 =
{
    .timestamp_s = 2400 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_3_2 =
{
    .timestamp_s = 2600 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_3_3 =
{
    .timestamp_s = 2800 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_3_4 =
{
    .timestamp_s = 3000 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_4_1 =
{
    .timestamp_s = 3200 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_4_2 =
{
    .timestamp_s = 3400 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_4_3 =
{
    .timestamp_s = 3600 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_4_4 =
{
    .timestamp_s = 3800 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};

const app_log_element_t e_5_1 =
{
    .timestamp_s = 4000 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_5_2 =
{
    .timestamp_s = 4200 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_5_3 =
{
    .timestamp_s = 4400 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_5_4 =
{
    .timestamp_s = 4600 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};

// 60 days
const app_log_element_t e_6_1 =
{
    .timestamp_s = 5184 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_6_2 =
{
    .timestamp_s = 5270 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_6_3 =
{
    .timestamp_s = 5357 * 1000,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};
const app_log_element_t e_6_4 =
{
    .timestamp_s = 54432 * 100,
    .temperature_c = 0,
    .humidity_rh = 0,
    .pressure_pa = 0
};

void setUp (void)
{
    ri_log_Ignore();
    rd_error_check_Ignore();
}

void tearDown (void)
{
}

extern app_log_record_t    m_log_input_block;
extern app_log_record_t    m_log_output_block;
extern app_log_config_t    m_log_config;
extern uint64_t            m_last_sample_ms;
extern uint16_t            m_boot_count;
#if RL_COMPRESS_ENABLED
extern rl_compress_state_t m_compress_state;
#endif

static void log_purge_Expect (void)
{
    for (uint8_t r_idx = 0; r_idx < APP_FLASH_LOG_DATA_RECORDS_NUM; r_idx++)
    {
        rd_status_t rvalue = RD_SUCCESS;

        if (r_idx % 2)
        {
            rvalue = RD_ERROR_NOT_FOUND;
        }

        rt_flash_free_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                       (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + r_idx,
                                       rvalue);
        rt_flash_busy_ExpectAndReturn (true);
        ri_yield_ExpectAndReturn (RD_SUCCESS);
        rt_flash_busy_ExpectAndReturn (false);
    }

    rt_flash_gc_run_ExpectAndReturn (RD_SUCCESS);
}

static void app_log_read_boot_count_Expect (void)
{
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE, APP_FLASH_LOG_BOOT_COUNTER_RECORD,
                                   &m_boot_count, sizeof (uint32_t), RD_SUCCESS);
    rt_flash_load_IgnoreArg_message();
    rt_flash_store_ExpectAndReturn (APP_FLASH_LOG_FILE, APP_FLASH_LOG_BOOT_COUNTER_RECORD,
                                    &m_boot_count, sizeof (uint32_t), RD_SUCCESS);
    rt_flash_store_IgnoreArg_message();
}

static void app_log_read_boot_count_not_found_Expect (void)
{
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE, APP_FLASH_LOG_BOOT_COUNTER_RECORD,
                                   &m_boot_count, sizeof (uint32_t), RD_ERROR_NOT_FOUND);
    rt_flash_load_IgnoreArg_message();
    rt_flash_store_ExpectAndReturn (APP_FLASH_LOG_FILE, APP_FLASH_LOG_BOOT_COUNTER_RECORD,
                                    &m_boot_count, sizeof (uint32_t), RD_SUCCESS);
    rt_flash_store_IgnoreArg_message();
}

static void app_log_read_boot_count_noflash_Expect (void)
{
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE, APP_FLASH_LOG_BOOT_COUNTER_RECORD,
                                   &m_boot_count, sizeof (uint32_t), RD_ERROR_INVALID_STATE);
    rt_flash_load_IgnoreArg_message();
}

static void sample_process_expect (const rd_sensor_data_t * const sample)
{
    if (APP_LOG_TEMPERATURE_ENABLED)
    {
        rd_sensor_data_parse_ExpectAndReturn (sample, RD_SENSOR_TEMP_FIELD, 0);
    }

    if (APP_LOG_HUMIDITY_ENABLED)
    {
        rd_sensor_data_parse_ExpectAndReturn (sample, RD_SENSOR_HUMI_FIELD, 0);
    }

    if (APP_LOG_PRESSURE_ENABLED)
    {
        rd_sensor_data_parse_ExpectAndReturn (sample, RD_SENSOR_PRES_FIELD, 0);
    }
}

static void sample_read_expect (rd_sensor_data_t * const sample,
                                const app_log_element_t * const p_el)
{
    if (APP_LOG_TEMPERATURE_ENABLED)
    {
        rd_sensor_data_set_Expect (sample, RD_SENSOR_TEMP_FIELD, p_el->temperature_c);
    }

    if (APP_LOG_HUMIDITY_ENABLED)
    {
        rd_sensor_data_set_Expect (sample, RD_SENSOR_HUMI_FIELD, p_el->humidity_rh);
    }

    if (APP_LOG_PRESSURE_ENABLED)
    {
        rd_sensor_data_set_Expect (sample, RD_SENSOR_PRES_FIELD, p_el->pressure_pa);
    }
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

static void store_block_expect_nomem (const uint8_t record_idx)
{
    rt_flash_free_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx,
                                   RD_SUCCESS);
    const bool block_flash = false;

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
                                    RD_ERROR_NO_MEM);

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

    store_block_expect (record_idx + 1, false);
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
#if APP_FLASH_LOG_CONFIG_NVM_ENABLED
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
#endif
    log_purge_Expect();
    app_log_read_boot_count_Expect();
    err_code |= app_log_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&defaults, &m_log_config, sizeof (m_log_config)));
}

#if APP_FLASH_LOG_CONFIG_NVM_ENABLED
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
    log_purge_Expect();
    app_log_read_boot_count_Expect();
    err_code |= app_log_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&stored, &m_log_config, sizeof (m_log_config)));
}
#endif

void test_app_log_init_noflash (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t defaults = {0};
    memcpy (&defaults, &m_log_config, sizeof (defaults));
#if APP_FLASH_LOG_CONFIG_NVM_ENABLED
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_CONFIG_RECORD,
                                   &defaults, sizeof (defaults),
                                   RD_ERROR_INVALID_STATE);
    rt_flash_load_IgnoreArg_message();
#   else
    log_purge_Expect();
#   endif
    app_log_read_boot_count_noflash_Expect();
    err_code |= app_log_init();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
    TEST_ASSERT (!memcmp (&defaults, &m_log_config, sizeof (m_log_config)));
}

void test_app_log_init_nostored_nocounter (void)
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
#if APP_FLASH_LOG_CONFIG_NVM_ENABLED
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
#endif
    log_purge_Expect();
    app_log_read_boot_count_not_found_Expect();
    err_code |= app_log_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&defaults, &m_log_config, sizeof (m_log_config)));
}

#if APP_FLASH_LOG_CONFIG_NVM_ENABLED
void test_app_log_init_stored_nocounter (void)
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
    log_purge_Expect();
    app_log_read_boot_count_not_found_Expect();
    err_code |= app_log_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&stored, &m_log_config, sizeof (m_log_config)));
}
#endif

void test_app_log_init_noflash_nocounter (void)
{
    rd_status_t err_code = RD_SUCCESS;
    app_log_config_t defaults = {0};
    memcpy (&defaults, &m_log_config, sizeof (defaults));
#if APP_FLASH_LOG_CONFIG_NVM_ENABLED
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   APP_FLASH_LOG_CONFIG_RECORD,
                                   &defaults, sizeof (defaults),
                                   RD_ERROR_INVALID_STATE);
    rt_flash_load_IgnoreArg_message();
#   else
    log_purge_Expect();
#   endif
    app_log_read_boot_count_noflash_Expect();
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
    sample_process_expect (&sample);
#   if RL_COMPRESS_ENABLED
    rl_compress_ExpectAndReturn (NULL,
                                 m_log_input_block.storage,
                                 sizeof (m_log_input_block.storage),
                                 &m_compress_state,
                                 RL_SUCCESS);
    rl_compress_IgnoreArg_data();
#   endif
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
            sample_process_expect (&sample);
#           if RL_COMPRESS_ENABLED
            rl_compress_ExpectAndReturn (NULL,
                                         m_log_input_block.storage,
                                         sizeof (m_log_input_block.storage),
                                         &m_compress_state,
                                         RL_SUCCESS);
            rl_compress_IgnoreArg_data();
#           endif
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
        for (size_t jj = 0; jj < STORED_FIELDS; jj++)
        {
            rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
        }

#       if RL_COMPRESS_ENABLED
        rl_compress_ExpectAndReturn (NULL,
                                     m_log_input_block.storage,
                                     sizeof (m_log_input_block.storage),
                                     &m_compress_state,
                                     RL_COMPRESS_END);
        rl_compress_IgnoreArg_data();
#       else
        m_log_input_block.num_samples = APP_LOG_MAX_SAMPLES;
#       endif
        bool store_fail = (ii) % 2;
        store_block_expect (record_idx, store_fail);
        record_idx ++;
        sample.timestamp_ms += (m_log_config.interval_s * 1001U);
        record_idx = record_idx % APP_FLASH_LOG_DATA_RECORDS_NUM;
        err_code |= app_log_process (&sample);
    }

    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_process_32b_ms_overflow (void)
{
    rd_status_t err_code = RD_SUCCESS;
    m_last_sample_ms = 0;
    float samples[4] = {0}; //!< number of fields to mock-store.
    rd_sensor_data_t sample =
    {
        .timestamp_ms = 5184000U * 1000U,
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
        for (size_t jj = 0; jj < STORED_FIELDS; jj++)
        {
            rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
        }

#       if RL_COMPRESS_ENABLED
        rl_compress_ExpectAndReturn (NULL,
                                     m_log_input_block.storage,
                                     sizeof (m_log_input_block.storage),
                                     &m_compress_state,
                                     RL_COMPRESS_END);
        rl_compress_IgnoreArg_data();
#       else
        m_log_input_block.num_samples = APP_LOG_MAX_SAMPLES;
#       endif
        bool store_fail = false;
        store_block_expect (record_idx, store_fail);
        record_idx ++;
        sample.timestamp_ms += (86400U * 1000U);
        record_idx = record_idx % APP_FLASH_LOG_DATA_RECORDS_NUM;
        err_code |= app_log_process (&sample);
    }

    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_process_nomem_block (void)
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
    m_log_input_block.num_samples = APP_LOG_MAX_SAMPLES;

    for (size_t ii = 0; ii < STORED_FIELDS; ii++)
    {
        rd_sensor_data_parse_ExpectAnyArgsAndReturn (0);
    }

#       if RL_COMPRESS_ENABLED
    rl_compress_ExpectAndReturn (NULL,
                                 m_log_input_block.storage,
                                 sizeof (m_log_input_block.storage),
                                 &m_compress_state,
                                 RL_COMPRESS_END);
    rl_compress_IgnoreArg_data();
#       else
    m_log_input_block.num_samples = APP_LOG_MAX_SAMPLES;
#       endif
    store_block_expect_nomem (record_idx);
    err_code = app_log_process (&sample);
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
        .timestamp_ms = 0U,
        .fields = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 1,
            .datas.pressure_pa = 1,
            .datas.voltage_v = 1
        },
        .valid = { 0 },
        .data = samples
    };
    app_log_record_t r1 =
    {
        .start_timestamp_s = 800  * 1000,
        .end_timestamp_s   = 1600 * 1000,
        .num_samples = 4,
        .storage = { e_1_1, e_1_2, e_1_3, e_1_4 }
    };
    app_log_read_state_t rs = {0};
    uint8_t record_idx = 0;
    // Load flash, check if we can find a block which has end timestamp after target time.
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx,
                                   &r1, sizeof (r1),
                                   RD_SUCCESS);
    rt_flash_load_IgnoreArg_message();
    rt_flash_load_ReturnArrayThruPtr_message (&r1, 1);
#if RL_COMPRESS_ENABLED
    uint32_t timestamp = sample.timestamp_ms / 1000;
    rl_data_t data = {0};
    rl_decompress_ExpectWithArrayAndReturn (&data, 1,
                                            m_log_output_block.storage, sizeof (m_log_output_block.storage),
                                            sizeof (m_log_output_block.storage),
                                            &m_compress_state, 1,
                                            &timestamp, 1,
                                            RD_SUCCESS);
#endif
    sample_read_expect (&sample, &r1.storage[0]);
    err_code |= app_log_read (&sample, &rs);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_read_null (void)
{
    rd_status_t err_code = RD_SUCCESS;
    rd_sensor_data_t sample = {0};
    app_log_read_state_t rs = {0};
    err_code = app_log_read (NULL, &rs);
    TEST_ASSERT (RD_ERROR_NULL == err_code);
    err_code = app_log_read (&sample, NULL);
    TEST_ASSERT (RD_ERROR_NULL == err_code);
    err_code = app_log_read (NULL, NULL);
    TEST_ASSERT (RD_ERROR_NULL == err_code);
}

void test_app_log_read_no_stored_data (void)
{
    rd_status_t err_code = RD_SUCCESS;
    rd_sensor_data_t sample = {0};
    app_log_read_state_t rs = {0};
    uint8_t record_idx = 0;

    for (; record_idx < APP_FLASH_LOG_DATA_RECORDS_NUM;)
    {
        rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                       (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx,
                                       &m_log_output_block, sizeof (m_log_output_block),
                                       RD_ERROR_NOT_FOUND);
        record_idx++;
    }

    err_code = app_log_read (&sample, &rs);
    TEST_ASSERT (RD_ERROR_NOT_FOUND == err_code);
}

void test_app_log_read_skip_old_data (void)
{
    rd_status_t err_code = RD_SUCCESS;
    rd_sensor_data_t sample = {0};
    app_log_read_state_t rs =
    {
        .oldest_element_ms = 1000 * 1000
    };
    app_log_record_t old = {0};
    app_log_record_t new =
    {
        .start_timestamp_s = 800  * 1000,
        .end_timestamp_s   = 1600 * 1000,
        .num_samples = 4,
        .storage = { e_1_1, e_1_2, e_1_3, e_1_4 }
    };
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U),
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&old, sizeof (old));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + 1U,
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&new, sizeof (new));
    sample_read_expect (&sample, &new.storage[0]);
    err_code = app_log_read (&sample, &rs);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_read_skip_missing_data (void)
{
    rd_status_t err_code = RD_SUCCESS;
    rd_sensor_data_t sample = {0};
    app_log_read_state_t rs =
    {
        .oldest_element_ms = 1000 * 1000
    };
    app_log_record_t old = {0};
    app_log_record_t new =
    {
        .start_timestamp_s = 3200  * 1000,
        .end_timestamp_s   = 4000 * 1000,
        .num_samples = 4,
        .storage = { e_4_1, e_4_2, e_4_3, e_4_4 }
    };
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U),
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&old, sizeof (old));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + 1U,
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_ERROR_NOT_FOUND);
    rt_flash_load_ReturnMemThruPtr_message (&new, sizeof (new));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + 2U,
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&new, sizeof (new));
    sample_read_expect (&sample, &new.storage[0]);
    err_code = app_log_read (&sample, &rs);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_read_out_of_sequence_data (void)
{
    rd_status_t err_code = RD_SUCCESS;
    rd_sensor_data_t sample = {0};
    app_log_read_state_t rs =
    {
        .oldest_element_ms = 1000 * 1000
    };
    app_log_record_t r1 =
    {
        .start_timestamp_s = 800  * 1000,
        .end_timestamp_s   = 1600 * 1000,
        .num_samples = 4,
        .storage = { e_1_1, e_1_2, e_1_3, e_1_4 }
    };
    app_log_record_t r2 =
    {
        .start_timestamp_s = 1600  * 1000,
        .end_timestamp_s   = 2400 * 1000,
        .num_samples = 4,
        .storage = { e_2_1, e_2_2, e_2_3, e_2_4 }
    };
    app_log_record_t r3 =
    {
        .start_timestamp_s = 2400  * 1000,
        .end_timestamp_s   = 3200 * 1000,
        .num_samples = 4,
        .storage = { e_3_1, e_3_2, e_3_3, e_3_4 }
    };
    app_log_record_t r4 =
    {
        .start_timestamp_s = 3200  * 1000,
        .end_timestamp_s   = 4000 * 1000,
        .num_samples = 4,
        .storage = { e_4_1, e_4_2, e_4_3, e_4_4 }
    };
    app_log_record_t * records[4] = {&r1, &r2, &r3, &r4};
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U),
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&r3, sizeof (r3));
    sample_read_expect (&sample, & (records[2]->storage[0]));
    sample_read_expect (&sample, & (records[2]->storage[1]));
    sample_read_expect (&sample, & (records[2]->storage[2]));
    sample_read_expect (&sample, & (records[2]->storage[3]));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + 1U,
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&r4, sizeof (r4));
    sample_read_expect (&sample, & (records[3]->storage[0]));
    sample_read_expect (&sample, & (records[3]->storage[1]));
    sample_read_expect (&sample, & (records[3]->storage[2]));
    sample_read_expect (&sample, & (records[3]->storage[3]));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + 2U,
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&r1, sizeof (r1));
    sample_read_expect (&sample, & (records[0]->storage[0]));
    sample_read_expect (&sample, & (records[0]->storage[1]));
    sample_read_expect (&sample, & (records[0]->storage[2]));
    sample_read_expect (&sample, & (records[0]->storage[3]));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + 3U,
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&r2, sizeof (r2));
    sample_read_expect (&sample, & (records[1]->storage[0]));
    sample_read_expect (&sample, & (records[1]->storage[1]));
    sample_read_expect (&sample, & (records[1]->storage[2]));
    sample_read_expect (&sample, & (records[1]->storage[3]));

    for (size_t ii = 4; ii < APP_FLASH_LOG_DATA_RECORDS_NUM; ii++)
    {
        rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                       (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + ii,
                                       &m_log_output_block, sizeof (m_log_output_block),
                                       RD_ERROR_NOT_FOUND);
    }

    uint8_t num_reads = 0;

    while (RD_SUCCESS == err_code)
    {
        err_code |= app_log_read (&sample, &rs);
        num_reads++;
    }

    TEST_ASSERT (RD_ERROR_NOT_FOUND == err_code);
    TEST_ASSERT (17 == num_reads);
}

void test_app_log_read_rambuffer_at_end (void)
{
    rd_status_t err_code = RD_SUCCESS;
    rd_sensor_data_t sample = {0};
    app_log_read_state_t rs =
    {
        .oldest_element_ms = 1000 * 1000
    };
    app_log_record_t r1 =
    {
        .start_timestamp_s = 800  * 1000,
        .end_timestamp_s   = 1600 * 1000,
        .num_samples = 4,
        .storage = { e_1_1, e_1_2, e_1_3, e_1_4 }
    };
    app_log_record_t r2 =
    {
        .start_timestamp_s = 1600  * 1000,
        .end_timestamp_s   = 2400 * 1000,
        .num_samples = 4,
        .storage = { e_2_1, e_2_2, e_2_3, e_2_4 }
    };
    app_log_record_t r3 =
    {
        .start_timestamp_s = 2400  * 1000,
        .end_timestamp_s   = 3200 * 1000,
        .num_samples = 4,
        .storage = { e_3_1, e_3_2, e_3_3, e_3_4 }
    };
    app_log_record_t r4 =
    {
        .start_timestamp_s = 3200  * 1000,
        .end_timestamp_s   = 4000 * 1000,
        .num_samples = 4,
        .storage = { e_4_1, e_4_2, e_4_3, e_4_4 }
    };
    app_log_record_t r5 =
    {
        .start_timestamp_s = 4000  * 1000,
        .end_timestamp_s   = 4800 * 1000,
        .num_samples = 4,
        .storage = { e_5_1, e_5_2, e_5_3, e_5_4 }
    };
    app_log_record_t * records[4] = {&r1, &r2, &r3, &r4};
    memcpy (&m_log_input_block, &r5, sizeof (app_log_record_t));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U),
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&r3, sizeof (r3));
    sample_read_expect (&sample, & (records[2]->storage[0]));
    sample_read_expect (&sample, & (records[2]->storage[1]));
    sample_read_expect (&sample, & (records[2]->storage[2]));
    sample_read_expect (&sample, & (records[2]->storage[3]));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + 1U,
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&r4, sizeof (r4));
    sample_read_expect (&sample, & (records[3]->storage[0]));
    sample_read_expect (&sample, & (records[3]->storage[1]));
    sample_read_expect (&sample, & (records[3]->storage[2]));
    sample_read_expect (&sample, & (records[3]->storage[3]));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + 2U,
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&r1, sizeof (r1));
    sample_read_expect (&sample, & (records[0]->storage[0]));
    sample_read_expect (&sample, & (records[0]->storage[1]));
    sample_read_expect (&sample, & (records[0]->storage[2]));
    sample_read_expect (&sample, & (records[0]->storage[3]));
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + 3U,
                                   &m_log_output_block, sizeof (m_log_output_block),
                                   RD_SUCCESS);
    rt_flash_load_ReturnMemThruPtr_message (&r2, sizeof (r2));
    sample_read_expect (&sample, & (records[1]->storage[0]));
    sample_read_expect (&sample, & (records[1]->storage[1]));
    sample_read_expect (&sample, & (records[1]->storage[2]));
    sample_read_expect (&sample, & (records[1]->storage[3]));

    for (size_t ii = 4; ii < APP_FLASH_LOG_DATA_RECORDS_NUM; ii++)
    {
        rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                       (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + ii,
                                       &m_log_output_block, sizeof (m_log_output_block),
                                       RD_ERROR_NOT_FOUND);
    }

    sample_read_expect (&sample, & (r5.storage[0]));
    sample_read_expect (&sample, & (r5.storage[1]));
    sample_read_expect (&sample, & (r5.storage[2]));
    sample_read_expect (&sample, & (r5.storage[3]));
    uint8_t num_reads = 0;

    while (RD_SUCCESS == err_code)
    {
        err_code |= app_log_read (&sample, &rs);
        num_reads++;
    }

    TEST_ASSERT (RD_ERROR_NOT_FOUND == err_code);
    TEST_ASSERT (21 == num_reads);
}

void test_app_log_read_32b_ms_overflow (void)
{
    rd_status_t err_code = RD_SUCCESS;
    float samples[NUM_FIELDS] = {0};
    rd_sensor_data_t sample =
    {
        .timestamp_ms = 0U,
        .fields = {
            .datas.temperature_c = 1,
            .datas.humidity_rh = 1,
            .datas.pressure_pa = 1,
            .datas.voltage_v = 1
        },
        .valid = { 0 },
        .data = samples
    };
    app_log_record_t r1 =
    {
        .start_timestamp_s = 5184  * 1000,
        .end_timestamp_s   = 54432 * 100,
        .num_samples = 4,
        .storage = { e_6_1, e_6_2, e_6_3, e_6_4 }
    };
    app_log_read_state_t rs = {0};
    uint8_t record_idx = 0;
    // Load flash, check if we can find a block which has end timestamp after target time.
    rt_flash_load_ExpectAndReturn (APP_FLASH_LOG_FILE,
                                   (APP_FLASH_LOG_DATA_RECORD_PREFIX << 8U) + record_idx,
                                   &r1, sizeof (r1),
                                   RD_SUCCESS);
    rt_flash_load_IgnoreArg_message();
    rt_flash_load_ReturnArrayThruPtr_message (&r1, 1);
#if RL_COMPRESS_ENABLED
    uint32_t timestamp = sample.timestamp_ms / 1000;
    rl_data_t data = {0};
    rl_decompress_ExpectWithArrayAndReturn (&data, 1,
                                            m_log_output_block.storage, sizeof (m_log_output_block.storage),
                                            sizeof (m_log_output_block.storage),
                                            &m_compress_state, 1,
                                            &timestamp, 1,
                                            RD_SUCCESS);
#endif
    sample_read_expect (&sample, &r1.storage[0]);
    err_code |= app_log_read (&sample, &rs);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_log_purge_flash (void)
{
    ri_flash_purge_Expect();
    app_log_purge_flash();
}

/** @} */
