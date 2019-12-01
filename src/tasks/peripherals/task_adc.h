#ifndef TASK_ADC_H
#define TASK_ADC_H

/**
 * @defgroup adc_tasks ADC tasks
 * @brief Internal Analog-to-digital converter control.
 *
 */
/*@}*/
/**
 * @addtogroup adc_tasks
 */
/*@{*/
/**
 * @file task_adc.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-11-28
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Read ADC. Important: The ADC peripheral is shared by many different functions
 * onboard, so always uninitialize the ADC after use to let others use the peripheral.
 * Likewise important: The ADC might be in use while you need it, so <b>always</b> check
 * the return code from init and <b>do no block</b> if you can't get ADC lock. 
 * 
 * ADC lock must be atomic, and it requires the ruuvi_interface_atomic implementation. 
 *
 * Typical usage:
 *
 * @code{.c}
 *  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
 *  err_code = task_adc_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;

 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;

 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;

 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = task_adc_uninit();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 * @endcode
 */

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_log.h"

typedef enum 
{
  RATIOMETRIC,    //!< ADC compares value to VDD
  ABSOLUTE        //!< ADC measures absolute voltage in volts
}task_adc_mode_t; //!< ADC against absolute reference or ratio to VDD

/**
 * @brief Reserve ADC
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is already initialized.
 */
ruuvi_driver_status_t task_adc_init (void);

/**
 * @brief Uninitialize ADC to release it for other users.
 *
 * @retval RUUVI_DRIVER_SUCCESS on success.
 * @retval RUUVI_DRIVER_ERROR_FATAL if ADC lock can't be released. Reboot.
 */
ruuvi_driver_status_t task_adc_uninit (void);

/**
 * @brief Check if ADC is initialized.
 *
 * @retval true if ADC is initialized.
 * @retval false if ADC is not initialized.
 */
bool task_adc_is_init (void);

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
ruuvi_driver_status_t task_adc_configure_se(ruuvi_driver_sensor_configuration_t* const config, const uint8_t handle, const task_adc_mode_t mode);

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
 * @param[in] data Data which has a field for absolute ADC value
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is not initialized or configured.
 * @retval error code from stack on error.
 */
ruuvi_driver_status_t task_adc_voltage_get (ruuvi_driver_sensor_data_t * const data);

/**
 * @brief Populate data with latest ratiometric value. 
 *
 * The data is ratio between 0.0 (gnd) and 1.0 (VDD). However the implementation is 
 * allowed to return negative values and values higher than 1.0 if the real voltage is 
 * beyond the supply rails or if differential sample is negative.
 *
 * @param[in] data Data which has a field for ratiometric ADC value
 * @retval RUUVI_DRIVER_SUCCESS on success
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if ADC is not initialized or configured.
 * @retval error code from stack on error.
 */
ruuvi_driver_status_t task_adc_ratio_get (ruuvi_driver_sensor_data_t * const data);

#endif // TASK_ADC_H
