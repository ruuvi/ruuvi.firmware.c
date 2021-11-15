/**
 * @file  application_config/application_mode_longmem.h
 * @author Otso Jousimaa <otso@ojousima.net>
 *         DGerman@Real-World-Systems.com
 * @date 2019-01-31
 * @brief   Override defaults from app_config.h.
 * Historical log will cover a longer time period then the default
 *   by saving samples less frequently,
 *   giving the appearance of more memory.
 *
 * The mode passed as a makefile flag : -DAPPLICAITON_MODE_LONGMEM.
 * This file is selected by application_modes.h
 *
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 *  EDIT THIS file to add additional customizations.
 */

#ifndef APPLICATION_MODE_LONGMEM_H
#define APPLICATION_MODE_LONGMEM_H

#define APP_FW_VARIANT "+longmem"

/** @brief interval between history log sample */
#define APP_LOG_INTERVAL_S     15 * 60     // 3 times the default.  15 minutes

/** @brief interval between advertisments */
#define APP_BLE_INTERVAL_MS   5 * 1285   // 5 times the default

/** @brief Number of times the advertisment is repeated */
#define APP_NUM_REPEATS              5      // 2 is the default


#endif // APPLICATION_MODE_LONGMEM_H
