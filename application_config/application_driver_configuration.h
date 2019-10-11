#ifndef APPLICATION_DRIVER_CONFIGURATION_H
#define APPLICATION_DRIVER_CONFIGURATION_H

#include "application_config.h"
#include "ruuvi_platform_nrf5_sdk15_config.h"
#include "nrf5_sdk15_application_config.h"

#define APPLICATION_BLE_SCAN_BUFFER_SIZE 31 //!< Bytes in BLE scanner buffer. 31 for normal advertisements, 256 for extended. 
#define APPLICATION_SCAN_INTERVAL_MS     10000
#define APPLICATION_SCAN_WINDOW_MS       100
#define APPLICATION_SCAN_TIMEOUT_MS      10000

#endif