#include "app_log.h"
#include "ruuvi_driver_sensor.h"
#include "unity.h"

void AssertEqual_rd_sensor_data_t (rd_sensor_data_t expected, rd_sensor_data_t actual,
                                   unsigned short line)
{
    UNITY_TEST_ASSERT_EQUAL_UINT64 (expected.timestamp_ms, actual.timestamp_ms, line,
                                    "rd_sensor_data_t timestamp failed");
    UNITY_TEST_ASSERT_EQUAL_UINT32 (expected.fields.bitfield, actual.fields.bitfield, line,
                                    "rd_sensor_data_t available fields failed");
    UNITY_TEST_ASSERT_EQUAL_UINT32 (expected.valid.bitfield, actual.valid.bitfield, line,
                                    "rd_sensor_data_t valid fields failed");
}

void AssertEqual_app_log_read_state_t (app_log_read_state_t expected,
                                       app_log_read_state_t actual, unsigned short line)
{
    UNITY_TEST_ASSERT_EQUAL_UINT64 (expected.oldest_element_ms, actual.oldest_element_ms,
                                    line,
                                    "app_log_read_state_t oldest_element_ms failed");
    UNITY_TEST_ASSERT_EQUAL_UINT16 (expected.element_idx, actual.element_idx, line,
                                    "app_log_read_state_t element_idx failed");
    UNITY_TEST_ASSERT_EQUAL_UINT8 (expected.page_idx, actual.page_idx, line,
                                   "app_log_read_state_t page_idx failed");
}