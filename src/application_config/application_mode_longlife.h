#ifndef APPLICATION_MODE_LONGLIFE_H
#define APPLICATION_MODE_LONGLIFE_H

#define APP_FW_VARIANT "+longlife"

#define APP_BLE_INTERVAL_MS (1285U * 7U) //8995 ms, longest allowed divisible by 1285

#define APP_NUM_REPEATS (7U) // ~63 s

#define APP_GATT_ENABLED (0U)

#define RT_FLASH_ENABLED (0U)

#endif