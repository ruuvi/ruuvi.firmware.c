/**
 * @file application_config/application_modes.h
 * @brief application mode definitions
 * mode MAYBE passed as a makefile flag ONE of: -DDEBUG | -DAPPLICATION_MODE_LONGLIFE | -DAPPLICAITON_MODE_LONGMEM
 *
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 * @author: Otso Jousimaa <otso@ojousima.net>
 *          DG12 <DGerman@Real-World-Systems.com>
 * @date 2021-07-24 add more comments and example
 *
 * Customizations do NOT go here
 */
#ifndef APPLICATION_MODES_H
#define APPLICATION_MODES_H

#ifdef APPLICATION_MODE_LONGLIFE
#include "application_mode_longlife.h"
#elif APPLICATION_MODE_LONGMEM
#include "application_mode_longmem.h"
#elif DEBUG
#include "application_mode_debug.h"
#endif

#include "application_mode_default.h"   // always add in the Default settings (perhaps superceeded by above)

#endif  //  APPLICATION_MODES_H



//  // // // // // // // // // // // // // // // // // // // //
// Example: debug:   (partial)
//
//  #define APP_FW_VARIANT "+debug"
//
//  #define RI_LOG_ENABLED 1
//  #define SEGGER_RTT_CONFIG_DEFAULT_MODE 2 // block 2 // trim 1 // skip 0 // if LOG buffer is full
//
//  #define APP_NUM_REPEATS                  4  // advertise the same data 4 times instead of default 1.
//  #define APP_LOG_INTERVAL_S             (1u) // save history every second rather than the default 5
//  #define APP_BLE_INTERVAL_MS          (221u) // advertise faster then default
//
// Example from default:  (partial)
//
//  #ifndef APP_BLE_INTERVAL_MS              // (Which is superceeded at  221 above)
//  #define APP_BLE_INTERVAL_MS (1285u)          /** advertise sensor data at this interval. 1285 matches Apple guideline. */
//  #endif
//
//END
