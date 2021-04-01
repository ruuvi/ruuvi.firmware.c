/**
 * @file  ruuvitag_b.ld
 * @author Otso Jousimaa <otso@ojousima.net>
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 * @date  2021-03-29
 * @modifications by DG12 to make it nice. No funcitonal changes.
 * @brief  Version 3.30.x for nrf52832
 * Linker script to configure memory regions and PROVIDE special symbols
 * MEMORY ORGINS must be listed in order 
 * RAM ORIGIN leaves room for Nordic SoftDevice 
 * storage_flash from 0006 0000 to 0007 5000 ( 15000 = 168 pages) immdiately below  bootloader
 * Noted here:
 *  kaarle  larger  FLASH
 *  keijo   smaller FLASH  and RAM
 *  kalervo smaller FLASH  and RAM
 * nrf_common.ld is included at the end 
 */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  uicr_bootloader_start_address (r) : ORIGIN = 0x00000FF8, LENGTH =     0x4     /* contains 75000 */
  uicr_mbr_params_page          (r) : ORIGIN = 0x00000FFC, LENGTH =     0x4     /* contains 7E000 */
  FLASH                        (rx) : ORIGIN = 0x00026000, LENGTH = 0x4F000     /*     26000 ..    75000  */
/*       kaarle                                                       52000                               */
/*       keijo                                                         A000                               */
/*       kalervo                                                       A000                               */
/* includes   storage_flash                         60000             15000                               */
  mbr_params_page               (r) : ORIGIN = 0x0007E000, LENGTH =  0x1000     /*     7E000 ..    7F000  */
  bootloader_settings_page      (r) : ORIGIN = 0x0007F000, LENGTH =  0x1000     /*     7F000 ..    80000  */

  RAM                         (rwx) : ORIGIN = 0x20002450, LENGTH =  0xDBB0     /*  20002450 .. 20010000  */
}
/* RAM   kaarle                                      2450              DBB0                               */
/* RAM   keijo                                       2D58              32A8 12KB                          */
/* RAM   kalervo                                     2D58              32A8 12KB                          */


SECTIONS
{
  . = ALIGN(4);
  .mem_section_dummy_ram : { }

  .log_dynamic_data : { PROVIDE(__start_log_dynamic_data = .); KEEP(*(SORT(.log_dynamic_data*))) PROVIDE(__stop_log_dynamic_data = .); } 
                        > RAM
   .log_filter_data :  { PROVIDE(__start_log_filter_data = .);   KEEP(*(SORT(.log_filter_data*)))  PROVIDE(__stop_log_filter_data = .); }
                        > RAM
  .fs_data :          { PROVIDE(__start_fs_data = .);           KEEP(*(.fs_data))                 PROVIDE(__stop_fs_data = .); } 
                        > RAM
} INSERT AFTER .data;

SECTIONS
{
 .mem_section_dummy_rom : { }
 .crypto_data :         { PROVIDE(__start_crypto_data = .);         KEEP(*(SORT(.crypto_data*)))        PROVIDE(__stop_crypto_data = .); } 
                         > FLASH
 .nrf_queue :           { PROVIDE(__start_nrf_queue = .);           KEEP(*(.nrf_queue))                 PROVIDE(__stop_nrf_queue = .); } 
                         > FLASH
 .dfu_trans :           { PROVIDE(__start_dfu_trans = .);           KEEP(*(SORT(.dfu_trans*)))          PROVIDE(__stop_dfu_trans = .); } 
                         > FLASH
 .svc_data :            { PROVIDE(__start_svc_data = .);            KEEP(*(.svc_data))                  PROVIDE(__stop_svc_data = .); } 
                         > FLASH
 .log_const_data :      { PROVIDE(__start_log_const_data = .);      KEEP(*(SORT(.log_const_data*)))     PROVIDE(__stop_log_const_data = .); } 
                         > FLASH
 .nrf_balloc :          { PROVIDE(__start_nrf_balloc = .);          KEEP(*(.nrf_balloc))                PROVIDE(__stop_nrf_balloc = .); } 
                         > FLASH
 .sdh_ble_observers :   { PROVIDE(__start_sdh_ble_observers = .);   KEEP(*(SORT(.sdh_ble_observers*)))  PROVIDE(__stop_sdh_ble_observers = .); } 
                         > FLASH
 .log_backends :        { PROVIDE(__start_log_backends = .);        KEEP(*(SORT(.log_backends*)))       PROVIDE(__stop_log_backends = .); } 
                         > FLASH
 .sdh_req_observers :   { PROVIDE(__start_sdh_req_observers = .);   KEEP(*(SORT(.sdh_req_observers*)))  PROVIDE(__stop_sdh_req_observers = .); } 
                         > FLASH
 .sdh_state_observers : { PROVIDE(__start_sdh_state_observers = .); KEEP(*(SORT(.sdh_state_observers*))) PROVIDE(__stop_sdh_state_observers = .); } 
                         > FLASH
 .sdh_stack_observers : { PROVIDE(__start_sdh_stack_observers = .); KEEP(*(SORT(.sdh_stack_observers*))) PROVIDE(__stop_sdh_stack_observers = .); } 
                         > FLASH
 .sdh_soc_observers :   { PROVIDE(__start_sdh_soc_observers = .);   KEEP(*(SORT(.sdh_soc_observers*)))   PROVIDE(__stop_sdh_soc_observers = .); } 
                         > FLASH
 .pwr_mgmt_data :       { PROVIDE(__start_pwr_mgmt_data = .);       KEEP(*(SORT(.pwr_mgmt_data*)))       PROVIDE(__stop_pwr_mgmt_data = .); } 
                         > FLASH
/*                       _______                                                                                                   i.e.  0007 5000 */
 .storage_flash 0x60000 (NOLOAD) : { __start_storage_flash = .;     KEEP(*(SORT(.storage_flash*)))              __stop_storage_flash = . + 0x15000; } 
                         > FLASH
/*    keijo  and kalervo do not have enough flash for storage_flash */

} INSERT AFTER .text;

INCLUDE "nrf_common.ld" 
