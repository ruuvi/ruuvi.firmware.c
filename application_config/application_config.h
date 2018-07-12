/** Define application constants, i.e. sample rates, whether ruuvi.endpoints libarary is in use **/

// <o> LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

// <o> INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

//#define RUUVI_ENDPOINTS
#ifndef APPLICATION_CONFIG
#define APPLICATION_CONFIG

#define MAIN_LOG_ENABLED 1
#define MAIN_LOG_LEVEL   3  //Info
#define MAIN_INFO_COLOR  0  //black 

#define BLE4_LOG_ENABLED 1
#define BLE4_LOG_LEVEL   3
#define BLE4_INFO_COLOR  0

#define SPI_PLATFORM_LOG_ENABLED 0
#define SPI_PLATFORM_LOG_LEVEL   3
#define SPI_PLATFORM_INFO_COLOR  0

#define I2C_PLATFORM_LOG_ENABLED 0
#define I2C_PLATFORM_LOG_LEVEL   3
#define I2C_PLATFORM_INFO_COLOR  0

#define LOG_PLATFORM_LOG_ENABLED    0
#define LOG_PLATFORM_LOG_LEVEL      3
#define LOG_PLATFORM_LOG_INFO_COLOR 0

#define UART_LOG_ENABLED    1
#define UART_LOG_LEVEL      3
#define UART_LOG_INFO_COLOR 0

#define ENVIRONMENTAL_SENSING 1
#define BME280_INTERFACE_LOG_ENABLED 0
#define BME280_INTERFACE_LOG_LEVEL   3  //Error
#define BME280_INTERFACE_INFO_COLOR  0  //black 

#define ACCELERATION_SENSING 1
#define LIS2DH12_INTERFACE_LOG_ENABLED 0
#define LIS2DH12_INTERFACE_LOG_LEVEL   3  //Error
#define LIS2DH12_INTERFACE_INFO_COLOR  0  //black 

// 24 bytes payload max for incoming/outgoing messages. 
// Note: BLE Mesh should be limited to 11 bytes, BLE4 GATT to 20.
#define RUUVI_COMMUNICATION_MESSAGE_MAX_PAYLOAD_LENGTH 24

#define APPLICATION_NFC 1
#define RUUVI_ENDPOINTS 1

// <i> The following modes are supported:
// <i> - SKIP  - Do not block, output nothing.
// <i> - TRIM  - Do not block, output as much as fits.
// <i> - BLOCK - Wait until there is space in the buffer.
// <0=> SKIP 
// <1=> TRIM 
// <2=> BLOCK_IF_FIFO_FULL 
#define SEGGER_RTT_CONFIG_DEFAULT_MODE 0

// Required by some drivers. If your platform does not support hw-floats, comment this and fix errors
#define APPLICATION_FLOAT_USE 1

#endif