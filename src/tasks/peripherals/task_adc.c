#include "task_adc.h"

#include <stdbool.h>

#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_adc_mcu.h"
#include "ruuvi_interface_atomic.h"

static ruuvi_interface_atomic_t m_is_init;
static bool m_is_configured;
static ruuvi_driver_sensor_t m_adc; //!< ADC control instance


/**
 * @brief Initialize ADC in low-power state.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is already initialized.
 */
ruuvi_driver_status_t task_adc_init (void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  if(!ruuvi_interface_atomic_flag(&m_is_init, true))
  {
    err_code |= RUUVI_DRIVER_ERROR_INVALID_STATE;
  }
  else
  {
    err_code |= ruuvi_interface_mcu_init(&m_adc);
    if(RUUVI_DRIVER_SUCCESS == err_code)
    {
      m_is_init = true;
    }
  }
  return err_code;
}

/**
 * @brief Uninitialize ADC to release it for other users.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is already initialized.
 */
ruuvi_driver_status_t task_adc_uninit (void)
{

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
ruuvi_driver_status_t task_adc_configure_se(const ruuvi_driver_sensor_configuration_t* const config, const uint8_t handle, const task_adc_mode mode)

/**
 * @brief Take a new sample on ADC configured in single-shot/sleep mode
 *
 * If this function returns RUUVI_DRIVER_SUCCESS new sample can be immediately read
 * with task_adc_voltage_get or task_adc_ratio_get
 *
 * @retval RUUVI_DRIVER_SUCCESS Sampling was successful
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE ADC is not initialized or configured
 */
ruuvi_driver_status_t task_adc_sample(void);

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
ruuvi_driver_status_t task_adc_voltage_get (ruuvi_interface_adc_data_t * const data);

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
ruuvi_driver_status_t task_adc_ratio_get (ruuvi_driver_sensor_data_t * const data);