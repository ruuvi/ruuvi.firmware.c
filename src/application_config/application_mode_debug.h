#ifndef APPLICATION_MODE_DEBUG_H
#define APPLICATION_MODE_DEBUG_H

#define APP_FW_VARIANT "+debug"
#define RI_LOG_ENABLED (true)
#define APP_LOG_LEVEL RI_LOG_LEVEL_INFO
#define APP_WDT_INTERVAL_MS (10U*60U*1000U)
#define ENABLE_ALL_DATAFORMATS (1U)

/** @brief Communicate sensor data at this interval. 221 matches Apple guideline. */
#define APP_BLE_INTERVAL_MS (221U)

/** @brief repeat same data N times in advertisement, reduce sensor reads. */
#define APP_NUM_REPEATS (4U)

/** @brief Communicate sensor data at this interval. 221 matches Apple guideline. */
#define APP_HEARTBEAT_INTERVAL_MS (APP_BLE_INTERVAL_MS * APP_NUM_REPEATS)

#define APP_LOG_INTERVAL_S (1U) //!< Gets limited to heartbeat rate.

#endif // APPLICATION_MODE_DEBUG_H
