#ifndef APP_DATAFORMATS_H
#define APP_DATAFORMATS_H

/**
 * @file app_dataformats.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2021-08-19
 * @copyright Ruuvi Innovations Ltd, License BSD-3-Clause.
 *
 * Helper to encode data into various formats
 *
 */

#include "app_config.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_3.h"
#include "ruuvi_endpoint_5.h"
#include "ruuvi_endpoint_8.h"
#include "ruuvi_endpoint_fa.h"

typedef enum
{
    DF_INVALID = 0U,
    DF_3       = (1U << 0U),
    DF_5       = (1U << 1U),
    DF_8       = (1U << 2U),
    DF_FA      = (1U << 3U)
} app_dataformat_t;

typedef struct
{
    unsigned int DF_3  : 1; //!< Dataformat 3 enabled
    unsigned int DF_5  : 1; //!< Dataformat 5 enabled
    unsigned int DF_8  : 1; //!< Dataformat 8 enabled
    unsigned int DF_FA : 1; //!< Dataformat fa enabled
} app_dataformats_t; //!< Container for enabled data formats


/**
 * @brief Return next dataformat to send
 *
 * @param[in] formats Enabled formats
 * @param[in] state Current state of dataformat picker.
 * @return    Next dataformat to use in app.
 */
app_dataformat_t app_dataformat_next (const app_dataformats_t formats,
                                      const app_dataformat_t state);

/**
 * @brief Encode data into given buffer with given format.
 *
 * A call to this function will increment measurement sequence counter
 * where applicable. Sensors are read to get latest data from board.
 *
 * @param[out] output Buffer to which data is encoded.
 * @param[in,out] output_length Input: Size of output buffer.
 *                              Output: Size of encoded data.
 * @param[in] data Pointer to sensor data to encode.
 * @param[in] format Format to encode data into.
 *
 */
rd_status_t app_dataformat_encode (uint8_t * const output,
                                   size_t * const output_length,
                                   const rd_sensor_data_t * const data,
                                   const app_dataformat_t format);

#endif // APP_DATAFORMATS_H
