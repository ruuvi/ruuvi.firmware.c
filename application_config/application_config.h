/** Define application constants, i.e. sample rates, whether ruuvi.endpoints libarary is in use **/

//#define RUUVI_ENDPOINTS
#define ENVIRONMENTAL_SENSING
#define BME280_ENVIRONMENTAL

#define ACCELERATION_SENSING
#define LIS2DH12_ACCELERATION

#define APPLICATION_NFC
#define NFC_TEXT_BUF_SIZE 128
#define NFC_URI_BUF_SIZE 64
#define NFC_APP_BUF_SIZE 64
#define NFC_DATA_BUF_SIZE 240
#define NDEF_FILE_SIZE 512
#define NFC_NDEF_MSG_PARSER_LOG_ENABLED 1
#define NFC_NDEF_MSG_PARSER_LOG_LEVEL   3 //INFO
#define NFC_NDEF_MSG_PARSER_INFO_COLOR  0 //Black
#define NFC_NDEF_RECORD_PARSER_LOG_ENABLED 1
#define NFC_NDEF_RECORD_PARSER_LOG_LEVEL 3
#define NFC_NDEF_RECORD_PARSER_INFO_COLOR 0

// <i> The following modes are supported:
// <i> - SKIP  - Do not block, output nothing.
// <i> - TRIM  - Do not block, output as much as fits.
// <i> - BLOCK - Wait until there is space in the buffer.
// <0=> SKIP 
// <1=> TRIM 
// <2=> BLOCK_IF_FIFO_FULL 
#define SEGGER_RTT_CONFIG_DEFAULT_MODE 2

#define APPLICATION_FLOAT_USE

