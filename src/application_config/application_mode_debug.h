#ifndef APPLICATION_MODE_DEBUG_H
#define APPLICATION_MODE_DEBUG_H

#define APP_FW_VARIANT "+debug"
#define RI_LOG_ENABLED 1
#define APP_LOG_LEVEL RI_LOG_LEVEL_INFO
#define APP_WDT_INTERVAL_MS (10U*60U*1000U)

/** @brief Communicate sensor data at this interval. 221 matches Apple guideline. */
#ifndef APP_BLE_INTERVAL_MS
#   define APP_BLE_INTERVAL_MS (221U)
#endif

/** @brief repeat same data N times in advertisement, reduce sensor reads. */
#ifndef APP_NUM_REPEATS
#   define APP_NUM_REPEATS 4
#endif

/** @brief Communicate sensor data at this interval. 221 matches Apple guideline. */
#ifndef APP_HEARTBEAT_INTERVAL_MS
#   define APP_HEARTBEAT_INTERVAL_MS (APP_BLE_INTERVAL_MS * APP_NUM_REPEATS)
#endif

#ifndef APP_LOG_INTERVAL_S
#   define APP_LOG_INTERVAL_S (1U) //!< Gets limited to heartbeat rate.
#endif

#endif