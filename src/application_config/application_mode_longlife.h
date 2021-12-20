/**
 * @file  application_config/application_mode_longlife.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2021-12-20
 * @brief   Override defaults from app_config.h.
 *       saving log less often then default and
 *       increasing BLE interval 7 times default
 *       will save battery life.
 * The mode passed as a makefile flag : -DAPPLICAITON_MODE_LONGLIFE.
 * This file is selected by application_modes.h
 *
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 *  EDIT THIS file to add additional customizations.
 */

#ifndef APPLICATION_MODE_LONGLIFE_H
#define APPLICATION_MODE_LONGLIFLIFE_H

#define APP_FW_VARIANT "+longlife"

#define APP_BLE_INTERVAL_MS (1285U * 7U) //8995 ms, longest allowed divisible by 1285

#define APP_NUM_REPEATS (7U) // ~63 s

/** @brief interval between history log sample */
#define APP_LOG_INTERVAL_S (15U * 60U) // 15 minutes

#endif // APPLICATION_MODE_LONGLIFE_H
