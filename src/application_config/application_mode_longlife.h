#ifndef APPLICATION_MODE_LONGLIFE_H
#define APPLICATION_MODE_LONGLIFE_H

#define APP_FW_VARIANT "+longlife"

#define APP_BLE_INTERVAL_MS (1285U * 7U) //8995 ms, longest allowed divisible by 1285

#define APP_NUM_REPEATS (1U) // ~9 s

#define APP_GATT_ENABLED (0U)

#define RT_FLASH_ENABLED (0U)

#define APP_SENSOR_LIS2DH12_SAMPLERATE (1U) //!< Hz

#define APP_SENSOR_TMP117_SAMPLERATE RD_SENSOR_CFG_CUSTOM_2 // 8000 ms conversion cycle.

#endif