/** #define sdk / platform dependencies **/
#include "application_config.h"

#define NRF_SDK14_SPI
#define NRF_SDK14_YIELD
#define NRF5_SDK14_PLATFORM

#ifdef APPLICATION_NFC
  #define NRF5_SDK14_NFC
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
#endif

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