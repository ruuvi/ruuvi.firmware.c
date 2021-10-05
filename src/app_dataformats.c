#include "app_dataformats.h"

#ifdef CEEDLING
#   define TESTABLE_STATIC
#else
#   define TESTABLE_STATIC static
#endif

/**
 * @brief Return next dataformat to send
 *
 * @param[in] formats Enabled formats
 * @param[in] state Current state of dataformat picker.
 * @return    Next dataformat to use in app.
 */
app_dataformat_t app_dataformat_next (const app_dataformats_t formats,
                                      const app_dataformat_t state)
{
    return DF_INVALID;
}

TESTABLE_STATIC rd_status_t
encode_to_3 (uint8_t * const output,
             size_t * const output_length,
             const rd_sensor_data_t * const data)
{
    return RD_ERROR_NOT_IMPLEMENTED;
}

TESTABLE_STATIC rd_status_t
encode_to_5 (uint8_t * const output,
             size_t * const output_length,
             const rd_sensor_data_t * const data)
{
    return RD_ERROR_NOT_IMPLEMENTED;
}

TESTABLE_STATIC rd_status_t
encode_to_8 (uint8_t * const output,
             size_t * const output_length,
             const rd_sensor_data_t * const data)
{
    return RD_ERROR_NOT_IMPLEMENTED;
}

TESTABLE_STATIC rd_status_t
encode_to_fa (uint8_t * const output,
              size_t * const output_length,
              const rd_sensor_data_t * const data)
{
    return RD_ERROR_NOT_IMPLEMENTED;
}

/**
 * @brief Encode data into given buffer with given format.
 *
 * A call to this function will increment measurement sequence counter
 * where applicable. Sensors are read to get latest data from board.
 *
 * @param[out] output Buffer to which data is encoded.
 * @param[in,out] output_length Input: Size of output buffer.
 *                              Output: Size of encoded data.
 * @param[in] format Format to encode data into.
 *
 */
rd_status_t app_dataformat_encode (uint8_t * const output,
                                   size_t * const output_length,
                                   const app_dataformat_t format)
{
    return RD_ERROR_NOT_IMPLEMENTED;
}