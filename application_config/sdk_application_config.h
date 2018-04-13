/** #define sdk / platform dependencies **/
#include "application_config.h"
#include "application_bluetooth_config.h"

// Use NRF5 SDK 14
#define NRF_SDK14 0
#if NRF_SDK14
  #define NRF5_SDK14_PLATFORM 1
  #define NRF_SDK14_SPI 1
  #define NRF_SDK14_YIELD 1
  #define NRF5_SDK14_NFC 1
  #define NRF_LOG_ENABLED 1
#endif

// Use NRF5 SDK 15
#define NRF_SDK15 1
#if NRF_SDK15
  #define NRF5_SDK15_PLATFORM   1
  #define NRF5_SDK15_BLE4_STACK 1
  #define NRF5_SDK15_BLE4_GATT  1
  #if NRF5_SDK15_BLE4_GATT
    #define PEER_MANAGER_ENABLED 1
    #define NRF_SDH_BLE_ENABLED 1
    #define NRF_BLE_GATT_ENABLED 1
    #define NRF_BLE_QWR_ENABLED 1
    #define BLE_DFU_ENABLED 1
    #define BLE_DIS_ENABLED 1
    #define BLE_NUS_ENABLED 1
    #define NRF_BLE_CONN_PARAMS_ENABLED 1
    // <o> NRF_BLE_CONN_PARAMS_MAX_SLAVE_LATENCY_DEVIATION - The largest acceptable deviation in slave latency. 
    // <i> The largest deviation (+ or -) from the requested slave latency that will not be renegotiated.
    #define NRF_BLE_CONN_PARAMS_MAX_SLAVE_LATENCY_DEVIATION 499
    // <o> NRF_BLE_CONN_PARAMS_MAX_SUPERVISION_TIMEOUT_DEVIATION - The largest acceptable deviation (in 10 ms units) in supervision timeout. 
    // <i> The largest deviation (+ or -, in 10 ms units) from the requested supervision timeout that will not be renegotiated.
    #define NRF_BLE_CONN_PARAMS_MAX_SUPERVISION_TIMEOUT_DEVIATION 65535

    #define NRF_SDH_BLE_PERIPHERAL_LINK_COUNT 1
    #define NRF_SDH_BLE_TOTAL_LINK_COUNT 1
    #define NRF_SDH_BLE_SERVICE_CHANGED 1
    #define NRF_SDH_BLE_VS_UUID_COUNT APPLICATION_SERVICE_COUNT
    #define NRF_SDH_BLE_GATTS_ATTR_TAB_SIZE 1408
  #endif
  #define NRF_SDK15_SPI 1
  #define SPI_ENABLED 1
  #define SPI0_ENABLED 1
  #define SPI0_USE_EASY_DMA 0

  #define NRF_SDK15_YIELD 1 
  #define NRF5_SDK15_GPIO 1
  #define NRF5_SDK15_TIMER 1
  #define NRF5_SDK15_PININTERRUPT 1
  #define FDS_ENABLED 1
  #define NRF_FSTORAGE_ENABLED 1

  #define NRF_LOG_ENABLED 1

  // #if APP_UART
  //   #define NRFX_UARTE_ENABLED 0
  //   #define NRFX_UART_ENABLED 1
  //   #define NRFX_UART0_ENABLED 1
  //   #define NRFX_UART0_ENABLED 1
  // #endif

  // <i> The following modes are supported:
  // <i> - SKIP  - Do not block, output nothing.
  // <i> - TRIM  - Do not block, output as much as fits.
  // <i> - BLOCK - Wait until there is space in the buffer.
  // <0=> SKIP 
  // <1=> TRIM 
  // <2=> BLOCK_IF_FIFO_FULL 
  #ifndef SEGGER_RTT_CONFIG_DEFAULT_MODE
  #define SEGGER_RTT_CONFIG_DEFAULT_MODE 0
  #endif

  #define NRF_LOG_DEFERRED 0
#endif  

#if APPLICATION_NFC
  #if NRF_SDK15
  #define NRF5_SDK15_NFC 1
  #define CLOCK_ENABLED 1 // Required for NFC
  #define NFC_NDEF_MSG_ENABLED 1
  #define NFC_NDEF_RECORD_ENABLED 1
  #define NFC_NDEF_URI_MSG_ENABLED 0
  #define NFC_NDEF_URI_REC_ENABLED 1
  #define NFC_NDEF_LAUNCHAPP_REC_ENABLED 1
  #define NFC_NDEF_TEXT_RECORD_ENABLED 1
  #define NFC_T4T_HAL_ENABLED 1
  #define NFC_NDEF_MSG_PARSER_ENABLED 1
  #define NFC_NDEF_RECORD_PARSER_ENABLED 1
  // <2=> Type 2 Tag 
  // <4=> Type 4 Tag 
  #define NFC_NDEF_MSG_TAG_TYPE 4
  //#define TIMER4 0.     // Timer 4 is reserved by NFC
  #define NFC_TEXT_BUF_SIZE 128
  #define NFC_URI_BUF_SIZE 64
  #define NFC_APP_BUF_SIZE 64
  #define NFC_DATA_BUF_SIZE 240
  #define NDEF_FILE_SIZE 512
  #define NFC_MAX_NUMBER_OF_RECORDS 4
  #define NFC_TAG_INTERFACE_LOG_ENABLED 1
  #define NFC_TAG_INTERFACE_LOG_LEVEL   3
  #define NFC_TAG_INTERFACE_LOG_COLOR   0
  #define NFC_NDEF_MSG_PARSER_LOG_ENABLED 0
  #define NFC_NDEF_MSG_PARSER_LOG_LEVEL   3 //INFO
  #define NFC_NDEF_MSG_PARSER_INFO_COLOR  0 //Black
  #define NFC_NDEF_RECORD_PARSER_LOG_ENABLED 0
  #define NFC_NDEF_RECORD_PARSER_LOG_LEVEL 3
  #define NFC_NDEF_RECORD_PARSER_INFO_COLOR 0
  #endif
#endif

