#include "unity.h"

#include "application_config.h"

#include "task_adc.h"

#include "ruuvi_driver_error.h"

#include "mock_ruuvi_interface_adc_mcu.h"
#include "mock_ruuvi_interface_atomic.h"
#include "mock_ruuvi_interface_log.h"

void setUp(void)
{
  ruuvi_interface_atomic_flag_IgnoreAndReturn(true);
  ruuvi_interface_adc_mcu_init_IgnoreAndReturn(RUUVI_DRIVER_SUCCESS);
  task_adc_set_init(true);
  TEST_ASSERT(task_adc_is_init());
}

void tearDown(void)
{
  ruuvi_interface_atomic_flag_IgnoreAndReturn(true);
  ruuvi_interface_adc_mcu_uninit_IgnoreAndReturn(RUUVI_DRIVER_SUCCESS);
  task_adc_set_init(false);
  TEST_ASSERT(!task_adc_is_init());
}

/**
 * @brief Initialize ADC in low-power state.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is already initialized.
 */
void test_task_adc_init_ok(void)
{
  tearDown();
  ruuvi_interface_atomic_flag_IgnoreAndReturn(true);
  ruuvi_interface_adc_mcu_init_IgnoreAndReturn(RUUVI_DRIVER_SUCCESS);
  TEST_ASSERT(task_adc_is_init());
}

void test_task_adc_init_busy(void)
{
  tearDown();
  ruuvi_interface_atomic_flag_IgnoreAndReturn(false);
  TEST_ASSERT(!task_adc_is_init());
}

void test_task_adc_init_error(void)
{
  tearDown();
  ruuvi_interface_atomic_flag_IgnoreAndReturn(true);
  ruuvi_interface_adc_mcu_init_IgnoreAndReturn(RUUVI_DRIVER_ERROR_INTERNAL);
  TEST_ASSERT(!task_adc_is_init());
}

/**
 * @brief Uninitialize ADC to release it for other users.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_FATAL if ADC lock can't be released. Reboot.
 */
void test_task_adc_uninit_ok(void)
{
  ruuvi_interface_atomic_flag_IgnoreAndReturn(true);
  ruuvi_interface_adc_mcu_uninit_IgnoreAndReturn(RUUVI_DRIVER_SUCCESS);
  TEST_ASSERT(!task_adc_is_init());
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
void test_task_adc_configure_se(void)
{
  TEST_IGNORE_MESSAGE("Implement");
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
void test_task_adc_sample(void)
{
  TEST_IGNORE_MESSAGE("Implement");
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
void test_task_adc_voltage_get (void)
{
  TEST_IGNORE_MESSAGE("Implement");
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
  TEST_IGNORE_MESSAGE("Implement");
}