#include "unity.h"

#include "application_config.h"

#include "task_adc.h"

#include "ruuvi_driver_error.h"

#include "mock_ruuvi_interface_adc_mcu.h"
#include "mock_ruuvi_interface_atomic.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_driver_sensor.h"

static volatile ruuvi_interface_atomic_t m_true = true;
static volatile ruuvi_interface_atomic_t m_false = false;

static ruuvi_driver_sensor_t m_sensor_init =
{
    .mode_set = ruuvi_interface_adc_mcu_mode_set,
    .configuration_set = ruuvi_driver_sensor_configuration_set,
    .data_get = ruuvi_interface_adc_mcu_data_get
};

static ruuvi_driver_sensor_t m_sensor_uninit;

static float m_valid_data[2] = {2.8F, 0.6F};
static ruuvi_driver_sensor_data_t m_adc_data =
{
    .data                       = m_valid_data,
    .fields.datas.voltage_v     = 1,
    .fields.datas.voltage_ratio = 1,
    .valid.datas.voltage_v      = 1,
    .valid.datas.voltage_ratio  = 1
};

void setUp (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_atomic_flag_ExpectAnyArgsAndReturn (true);
    ruuvi_interface_atomic_flag_ReturnThruPtr_flag (&m_true);
    err_code = task_adc_init();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (task_adc_is_init());
}

void tearDown (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_adc_mcu_uninit_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_atomic_flag_ExpectAnyArgsAndReturn (true);
    ruuvi_interface_atomic_flag_ReturnThruPtr_flag (&m_false);
    err_code = task_adc_uninit();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (!task_adc_is_init());
}

/**
 * @brief Initialize ADC in low-power state.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is already initialized.
 */
void test_task_adc_init_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    tearDown();
    ruuvi_interface_atomic_flag_ExpectAnyArgsAndReturn (true);
    ruuvi_interface_atomic_flag_ReturnThruPtr_flag (&m_true);
    err_code = task_adc_init();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (task_adc_is_init());
}

void test_task_adc_init_busy (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    tearDown();
    ruuvi_interface_atomic_flag_ExpectAnyArgsAndReturn (false);
    err_code = task_adc_init();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
    TEST_ASSERT (!task_adc_is_init());
}

/**
 * @brief Uninitialize ADC to release it for other users.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_FATAL if ADC lock can't be released. Reboot.
 */
void test_task_adc_uninit_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_interface_adc_mcu_uninit_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_atomic_flag_ExpectAnyArgsAndReturn (true);
    ruuvi_interface_atomic_flag_ReturnThruPtr_flag (&m_false);
    err_code = task_adc_uninit();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (!task_adc_is_init());
}

/**
 * @brief Configure ADC before sampling
 *
 * This function readies the ADC for sampling.
 * Configuring the ADC may take some time (< 1 ms) while actual sample must be as fast.
 * as possible to catch transients.
 *
 * <b>Note:</b> ADC should be configured to sleep or continuous mode. To take a single sample,
 * call @ref task_adc_sample_se after configuration. Configuring ADC into single sample mode is
 * equivalent to configuring ADC into sleep and then calling @ref task_adc_sample_se immediately
 *
 * @param[in, out] config Configuration of ADC.
 * @param[in] handle Handle to ADC, i.e. ADC pin.
 * @param[in] mode sampling mode, absolute or ratiometric
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is not initialized or if it is already configured.
 */
void test_task_adc_configure_se_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_driver_sensor_configuration_t config = {0};
    ruuvi_interface_adc_mcu_init_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_adc_mcu_init_ReturnArrayThruPtr_adc_sensor (&m_sensor_init, 1);
    ruuvi_driver_sensor_configuration_set_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    err_code = task_adc_configure_se (&config, RUUVI_INTERFACE_ADC_AINVDD, ABSOLUTE);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_adc_configure_se_twice (void)
{
    test_task_adc_configure_se_ok();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_driver_sensor_configuration_t config = {0};
    err_code = task_adc_configure_se (&config, RUUVI_INTERFACE_ADC_AINVDD, ABSOLUTE);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

void test_task_adc_configure_se_not_init (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_driver_sensor_configuration_t config = {0};
    err_code = task_adc_configure_se (&config, RUUVI_INTERFACE_ADC_AINVDD, ABSOLUTE);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

/**
 * @brief Take a new sample on ADC configured in single-shot/sleep mode
 *
 * If this function returns RUUVI_DRIVER_SUCCESS new sample can be immediately read
 * with task_adc_voltage_get or task_adc_ratio_get
 *
 * @retval RUUVI_DRIVER_SUCCESS Sampling was successful
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE ADC is not initialized or configured
 */
void test_task_adc_sample_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    test_task_adc_configure_se_ok();
    uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
    ruuvi_interface_adc_mcu_mode_set_ExpectAndReturn (&mode,
            RUUVI_DRIVER_SUCCESS);
    err_code = task_adc_sample();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_adc_sample_not_configured (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_adc_sample();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

void test_task_adc_sample_not_initialized (void)
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_adc_sample();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

/**
 * @brief Populate data with latest sample.
 *
 * The data is absolute voltage relative to device ground.
 *
 * @param[in] handle Handle for ADC peripheral, e.g. ADC number
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is not initialized or configured.
 * @retval error code from stack on error.
 */
void test_task_adc_voltage_get_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    float data[2] = {0};
    ruuvi_driver_sensor_data_t adc_data;
    adc_data.data = data;
    adc_data.fields.datas.voltage_v = 1;
    test_task_adc_sample_ok();
    ruuvi_interface_adc_mcu_data_get_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_adc_mcu_data_get_ReturnArrayThruPtr_data (&m_adc_data, 1);
    err_code = task_adc_voltage_get (&adc_data);
    TEST_ASSERT (true == adc_data.valid.datas.voltage_v);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

/**
 * @brief Prepare for sampling VDD
 *
 * This function should be called before entering energy intensive activity, such as using radio to transmit data.
 * After calling this function ADC is primed for measuring the voltage droop of battery.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_BUSY if ADC cannot be reserved
 */
void test_task_adc_vdd_prepare_ok (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_driver_sensor_configuration_t config = {0};
    tearDown();
    ruuvi_interface_atomic_flag_ExpectAnyArgsAndReturn (true);
    ruuvi_interface_atomic_flag_ReturnThruPtr_flag (&m_true);
    ruuvi_interface_adc_mcu_init_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_adc_mcu_init_ReturnArrayThruPtr_adc_sensor (&m_sensor_init, 1);
    ruuvi_driver_sensor_configuration_set_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    err_code = task_adc_vdd_prepare ();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_adc_vdd_prepare_already_init (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    ruuvi_driver_sensor_configuration_t config = {0};
    tearDown();
    ruuvi_interface_atomic_flag_ExpectAnyArgsAndReturn (true);
    ruuvi_interface_atomic_flag_ReturnThruPtr_flag (&m_false);
    err_code = task_adc_vdd_prepare ();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_BUSY == err_code);
}

/**
 * @brief Sample VDD
 *
 * This function should be called as soon as possible after energy intensive activity.
 * After a successful call value returned by @ref task_adc_vdd_get is updated and ADC is released.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if task_adc_vdd_prepare wasn't called.
 */
void test_task_adc_vdd_sample_ok (void)
{
    test_task_adc_vdd_prepare_ok();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
    ruuvi_interface_adc_mcu_mode_set_ExpectAndReturn (&mode,
            RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_adc_mcu_data_get_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_adc_mcu_data_get_ReturnArrayThruPtr_data (&m_adc_data, 1);
    ruuvi_driver_sensor_data_parse_ExpectAnyArgsAndReturn (m_valid_data[0]);
    ruuvi_interface_adc_mcu_uninit_ExpectAnyArgsAndReturn (RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_atomic_flag_ExpectAnyArgsAndReturn (true);
    ruuvi_interface_atomic_flag_ReturnThruPtr_flag (&m_false);
    err_code = task_adc_vdd_sample();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_adc_vdd_sample_not_prepared (void)
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code = task_adc_vdd_sample();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

/**
 * @brief Get VDD
 *
 * This function should be called any time after @ref task_adc_vdd_prepare.
 * The value returned will remain fixed until next call to @ref task_adc_vdd_prepare.
 *
 * @param[out] vdd VDD voltage in volts.
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if task_adc_vdd_sample wasn't called.
 */
void test_task_adc_vdd_get_ok (void)
{
    ruuvi_driver_sensor_data_t * const battery;
    test_task_adc_vdd_prepare_ok();
    test_task_adc_vdd_sample_ok();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    float data[2] = {0};
    ruuvi_driver_sensor_data_t adc_data;
    adc_data.data = data;
    adc_data.fields.datas.voltage_v = 1;
    err_code = task_adc_vdd_get (&adc_data);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

void test_task_adc_vdd_get_not_sampled (void)
{
    ruuvi_driver_sensor_data_t * const battery;
    test_task_adc_vdd_prepare_ok();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    float data[2] = {0};
    ruuvi_driver_sensor_data_t adc_data;
    adc_data.data = data;
    adc_data.fields.datas.voltage_v = 1;
    err_code = task_adc_vdd_get (&adc_data);
    TEST_ASSERT (false == adc_data.valid.datas.voltage_v);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

/**
 * @brief Populate data with latest ratiometric value.
 *
 * The data is ratio between 0.0 (gnd) and 1.0 (VDD). However the implementation is
 * allowed to return negative values and values higher than 1.0 if the real voltage is
 * beyond the supply rails or if differential sample is negative.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is not initialized or configured.
 * @retval error code from stack on error.
 */
void test_task_adc_ratio_get (void)
{
    TEST_IGNORE_MESSAGE ("Implement");
}