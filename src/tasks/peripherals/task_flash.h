#ifndef  TASK_FLASH_H
#define  TASK_FLASH_H

/**
 * @defgroup peripheral_tasks Peripheral tasks
 */
/*@{*/
/**
 * @defgroup flash_tasks Flash tasks
 * @brief Non-volatile storage functions.
 *
 */
/*@}*/
/**
 * @addtogroup flash_tasks
 */
/*@{*/
/**
 * @file task_flash.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-10-11
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 *
 * Store and load data to/from persistent storage.
 * Typical usage:
 *
 * @code{.c}
 *  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
 *  err_code = task_flash_init();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  char data[] = "Hello Flash!"
 *  err_code = task_flash_store(1, 1, data, sizeof(data));
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  while(task_flash_busy());
 *  char load[20];
 *  err_code = task_flash_load(1, 1, load, sizeof(data));
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  while(task_flash_busy());
 *  err_code = task_flash_free(1, 1);
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 *  err_code = task_flash_gc_run();
 *  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS;
 * @endcode
 */

#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_flash.h"
#include "ruuvi_interface_log.h"

/**
 * @brief Initialize flash storage.
 *
 * If flash initialization fails, flash is purged and device tries to enter bootloader.
 *
 * @return RUUVI_DRIVER_SUCCESS on success
 * @return error code from stack on error
 * @warning Erases entire flash storage and reboots on failure.
 */
ruuvi_driver_status_t task_flash_init(void);

/**
 * @brief Store data to flash.
 *
 * The flash storage implements a simple file system, where data is arranged to files which have records.
 * You can for example have a file for sensor configurations and record for each sensor. Underlying implementation
 * provides wear leveling. Garbage collection may be triggered manually and is tried automatically if there is not enough space in flash.
 * This function only queues the data to be written, you must verify that write was completed before freeing message.
 *
 * @param[in] file_id ID of a file to store. Valid range 1 ... 0xBFFF
 * @param[in] record_id ID of a record to store. Valid range 1 ... 0x
 * @param[in] message Data to store. Must be aligned to a 4-byte boundary.
 * @param[in] message_length Length of stored data. Maximum 4000 bytes per record on nRF52.
 *
 * @return RUUVI_DRIVER_SUCCESS on success.
 * @return RUUVI_DRIVER_ERROR_NULL if data pointer is NULL.
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if flash is not initialized.
 * @return RUUVI_DRIVER_ERROR_BUSY if another operation was ongoing.
 * @return RUUVI_DRIVER_ERROR_NO_MEM if there was no space for the record in flash.
 * @return RUUVI_DRIVER_ERROR_DATA_SIZE if record exceeds maximum size.
 *
 * @warning triggers garbage collection if there is no space available, which leads to long processing time.
 */
ruuvi_driver_status_t task_flash_store(const uint16_t file_id, const uint16_t record_id,
                                       const void* const message, const size_t message_length);

/**
 * @brief Load data from flash.
 *
 * The flash storage implements a simple file system, where data is arranged to files which have records.
 * You can for example have a file for sensor configurations and record for each sensor.
 *
 * @param[in] file_id ID of a file to load. Valid range 1 ... 0xBFFF
 * @param[in] record_id ID of a record to load. Valid range 1 ... 0x
 * @param[in] message Data to load. Must be aligned to a 4-byte boundary.
 * @param[in] message_length Length of loaded data. Maximum 4000 bytes per record on nRF52.
 *
 * @return RUUVI_DRIVER_SUCCESS on success.
 * @return RUUVI_DRIVER_ERROR_NULL if data pointer is NULL.
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if flash is not initialized.
 * @return RUUVI_DRIVER_ERROR_BUSY if another operation was ongoing.
 * @return RUUVI_DRIVER_ERROR_NOT_FOUND if given record was not found.
 * @return RUUVI_DRIVER_ERROR_DATA_SIZE if record exceeds maximum size.
 *
 * @warning triggers garbage collection if there is no space available, which leads to long processing time.
 */
ruuvi_driver_status_t task_flash_load(const uint16_t file_id, const uint16_t record_id,
                                      void* const message, const size_t message_length);

/**
 * @brief Free data from flash.
 *
 * The flash storage implements a simple file system, where data is arranged to files which have records.
 * You can for example have a file for sensor configurations and record for each sensor.
 *
 * This function does not physically erase the data, it only marks the record as deleteable for garbage collection.
 *
 * @param[in] file_id ID of a file to delete. Valid range 1 ... 0xBFFF
 * @param[in] record_id ID of a record to delete. Valid range 1 ... 0x
 *
 * @return RUUVI_DRIVER_SUCCESS on success.
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if flash is not initialized.
 * @return RUUVI_DRIVER_ERROR_BUSY if another operation was ongoing.
 * @return RUUVI_DRIVER_ERROR_NOT_FOUND if given record was not found.
 *
 * @warning triggers garbage collection if there is no space available, which leads to long processing time.
 */
ruuvi_driver_status_t task_flash_free(const uint16_t file_id, const uint16_t record_id);

/**
 * @brief Trigger garbage collection
 *
 * Free up space in flash by erasing old records. It's generally a good idea to check remaining
 * space on flash after a write and trigger GC if the space remaining is smaller than expected record sizes.
 * This function physically erases the records which are garbage collected.
 * Waits until GC can be triggered, and returns when GC operation has been queued.
 *
 *
 * @return RUUVI_DRIVER_SUCCESS on success.
 * @return RUUVI_DRIVER_ERROR_INVALID_STATE if flash is not initialized.
 *
 */
ruuvi_driver_status_t task_flash_gc_run(void);

/**
 * @brief Check if flash is running an operation.
 *
 * @return True if flash is busy.
 * @return False otherwise.
 *
 */
bool task_flash_busy(void);
/*@}*/
#endif