# Source files and includes common for all targets
NRF_LIB_SOURCES= \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c \
  $(SDK_ROOT)/components/ble/ble_radio_notification/ble_radio_notification.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis/ble_dis.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus/ble_nus.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_svci.c \
  $(SDK_ROOT)/components/libraries/crc16/crc16.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_init.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng_mbedtls.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_aes.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_aes_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_ecdh.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_hmac.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_init.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aes_shared.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aes.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_rng.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_error.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hkdf.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hmac.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/serial/nrf_serial.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/external/mbedtls/library/aes.c \
  $(SDK_ROOT)/external/mbedtls/library/aesni.c \
  $(SDK_ROOT)/external/mbedtls/library/arc4.c \
  $(SDK_ROOT)/external/mbedtls/library/asn1parse.c \
  $(SDK_ROOT)/external/nrf_tls/mbedtls/replacements/asn1write.c \
  $(SDK_ROOT)/external/mbedtls/library/base64.c \
  $(SDK_ROOT)/external/mbedtls/library/bignum.c \
  $(SDK_ROOT)/external/mbedtls/library/blowfish.c \
  $(SDK_ROOT)/external/mbedtls/library/camellia.c \
  $(SDK_ROOT)/external/mbedtls/library/ccm.c \
  $(SDK_ROOT)/external/mbedtls/library/certs.c \
  $(SDK_ROOT)/external/mbedtls/library/cipher.c \
  $(SDK_ROOT)/external/mbedtls/library/cipher_wrap.c \
  $(SDK_ROOT)/external/mbedtls/library/cmac.c \
  $(SDK_ROOT)/external/mbedtls/library/ctr_drbg.c \
  $(SDK_ROOT)/external/mbedtls/library/debug.c \
  $(SDK_ROOT)/external/mbedtls/library/des.c \
  $(SDK_ROOT)/external/mbedtls/library/dhm.c \
  $(SDK_ROOT)/external/mbedtls/library/ecdh.c \
  $(SDK_ROOT)/external/mbedtls/library/ecdsa.c \
  $(SDK_ROOT)/external/mbedtls/library/ecp.c \
  $(SDK_ROOT)/external/mbedtls/library/ecp_curves.c \
  $(SDK_ROOT)/external/mbedtls/library/entropy.c \
  $(SDK_ROOT)/external/mbedtls/library/entropy_poll.c \
  $(SDK_ROOT)/external/mbedtls/library/error.c \
  $(SDK_ROOT)/external/mbedtls/library/gcm.c \
  $(SDK_ROOT)/external/mbedtls/library/havege.c \
  $(SDK_ROOT)/external/mbedtls/library/hmac_drbg.c \
  $(SDK_ROOT)/external/mbedtls/library/md.c \
  $(SDK_ROOT)/external/mbedtls/library/md2.c \
  $(SDK_ROOT)/external/mbedtls/library/md4.c \
  $(SDK_ROOT)/external/mbedtls/library/md5.c \
  $(SDK_ROOT)/external/mbedtls/library/md_wrap.c \
  $(SDK_ROOT)/external/mbedtls/library/memory_buffer_alloc.c \
  $(SDK_ROOT)/external/mbedtls/library/oid.c \
  $(SDK_ROOT)/external/mbedtls/library/padlock.c \
  $(SDK_ROOT)/external/mbedtls/library/pem.c \
  $(SDK_ROOT)/external/mbedtls/library/pk.c \
  $(SDK_ROOT)/external/mbedtls/library/pk_wrap.c \
  $(SDK_ROOT)/external/mbedtls/library/pkcs11.c \
  $(SDK_ROOT)/external/mbedtls/library/pkcs12.c \
  $(SDK_ROOT)/external/mbedtls/library/pkcs5.c \
  $(SDK_ROOT)/external/mbedtls/library/pkparse.c \
  $(SDK_ROOT)/external/mbedtls/library/pkwrite.c \
  $(SDK_ROOT)/external/mbedtls/library/platform.c \
  $(SDK_ROOT)/external/mbedtls/library/ripemd160.c \
  $(SDK_ROOT)/external/mbedtls/library/rsa.c \
  $(SDK_ROOT)/external/mbedtls/library/sha1.c \
  $(SDK_ROOT)/external/mbedtls/library/sha256.c \
  $(SDK_ROOT)/external/mbedtls/library/sha512.c \
  $(SDK_ROOT)/external/mbedtls/library/ssl_cache.c \
  $(SDK_ROOT)/external/mbedtls/library/ssl_ciphersuites.c \
  $(SDK_ROOT)/external/mbedtls/library/ssl_cli.c \
  $(SDK_ROOT)/external/mbedtls/library/ssl_cookie.c \
  $(SDK_ROOT)/external/nrf_tls/mbedtls/replacements/ssl_srv.c \
  $(SDK_ROOT)/external/mbedtls/library/ssl_ticket.c \
  $(SDK_ROOT)/external/mbedtls/library/ssl_tls.c \
  $(SDK_ROOT)/external/mbedtls/library/threading.c \
  $(SDK_ROOT)/external/mbedtls/library/version.c \
  $(SDK_ROOT)/external/mbedtls/library/version_features.c \
  $(SDK_ROOT)/external/mbedtls/library/x509.c \
  $(SDK_ROOT)/external/mbedtls/library/x509_create.c \
  $(SDK_ROOT)/external/mbedtls/library/x509_crl.c \
  $(SDK_ROOT)/external/mbedtls/library/x509_crt.c \
  $(SDK_ROOT)/external/mbedtls/library/x509_csr.c \
  $(SDK_ROOT)/external/mbedtls/library/xtea.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_rng.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_spi.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_twi.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_power.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_pwm.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rng.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rtc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spim.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_timer.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twim.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c

NFC_SOURCES= \
  $(SDK_ROOT)/components/nfc/ndef/generic/message/nfc_ndef_msg.c \
  $(SDK_ROOT)/components/nfc/ndef/generic/record/nfc_ndef_record.c \
  $(SDK_ROOT)/components/nfc/ndef/parser/message/nfc_ndef_msg_parser.c \
  $(SDK_ROOT)/components/nfc/ndef/parser/message/nfc_ndef_msg_parser_local.c \
  $(SDK_ROOT)/components/nfc/ndef/parser/record/nfc_ndef_record_parser.c \
  $(SDK_ROOT)/components/nfc/ndef/launchapp/nfc_launchapp_rec.c \
  $(SDK_ROOT)/components/nfc/ndef/text/nfc_text_rec.c \
  $(SDK_ROOT)/components/nfc/ndef/uri/nfc_uri_msg.c \
  $(SDK_ROOT)/components/nfc/ndef/uri/nfc_uri_rec.c \
  $(SDK_ROOT)/components/nfc/platform/nfc_platform.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi.nrf_sdk15_3_overrides.c/nrfx_nfct.c

NFC_T4T_FPU_BIN= \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi.nrf_sdk15_3_overrides.c/t4t_lib/nfc_t4t_lib_gcc.a

RUUVI_LIB_SOURCES= \
  $(PROJ_DIR)/ruuvi.drivers.c/BME280_driver/bme280.c \
  $(PROJ_DIR)/ruuvi.drivers.c/BME280_driver/selftest/bme280_selftest.c \
  $(PROJ_DIR)/ruuvi.drivers.c/ruuvi.dps310.c/src/dps310.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_driver_sensor_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_driver_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_communication_ble_advertising_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_communication_ble_gatt_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_communication_nfc_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_communication_radio_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_communication_uart_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_flash_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_gpio_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_gpio_interrupt_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_gpio_pwm_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_power_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_scheduler_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests/ruuvi_interface_timer_test.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/acceleration/ruuvi_interface_lis2dh12.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/communication/ruuvi_interface_communication_radio.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/environmental/ruuvi_interface_adc_ntc.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/environmental/ruuvi_interface_adc_photo.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/environmental/ruuvi_interface_bme280.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/environmental/ruuvi_interface_dps310.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/environmental/ruuvi_interface_scd41.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/environmental/ruuvi_interface_sen55.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/environmental/ruuvi_interface_shtcx.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/environmental/ruuvi_interface_tmp117.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/i2c/ruuvi_interface_i2c_bme280.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/i2c/ruuvi_interface_i2c_shtcx.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/i2c/ruuvi_interface_i2c_tmp117.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/log/ruuvi_interface_log.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/spi/ruuvi_interface_spi_bme280.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/spi/ruuvi_interface_spi_dps310.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/spi/ruuvi_interface_spi_lis2dh12.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/adc/ruuvi_nrf5_sdk15_adc_mcu.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/atomic/ruuvi_nrf5_sdk15_atomic.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/communication/ruuvi_nrf5_sdk15_communication.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/communication/ruuvi_nrf5_sdk15_communication_ble_advertising.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/communication/ruuvi_nrf5_sdk15_communication_ble_gatt.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/communication/ruuvi_nrf5_sdk15_communication_nfc.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/communication/ruuvi_nrf5_sdk15_communication_radio.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/communication/ruuvi_nrf5_sdk15_communication_uart.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/crypto/ruuvi_nrf5_sdk15_aes.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/environmental/ruuvi_nrf5_sdk15_environmental_mcu.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/flash/ruuvi_nrf5_sdk15_flash.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/gpio/ruuvi_nrf5_sdk15_gpio.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/gpio/ruuvi_nrf5_sdk15_gpio_interrupt.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/gpio/ruuvi_nrf5_sdk15_gpio_pwm.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/i2c/ruuvi_nrf5_sdk15_i2c.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/log/ruuvi_nrf5_sdk15_log.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/power/ruuvi_nrf5_sdk15_power.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/rtc/ruuvi_nrf5_sdk15_rtc_mcu.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi_nrf5_sdk15_error.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi.nrf_sdk15_3_overrides.c/ble_dfu.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi.nrf_sdk15_3_overrides.c/ble_dfu_bonded.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi.nrf_sdk15_3_overrides.c/ble_dfu_unbonded.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi.nrf_sdk15_3_overrides.c/fds.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi.nrf_sdk15_3_overrides.c/nrf_ble_scan.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi.nrf_sdk15_3_overrides.c/nrfx_wdt.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/scheduler/ruuvi_nrf5_sdk15_scheduler.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/spi/ruuvi_nrf5_sdk15_spi.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/timer/ruuvi_nrf5_sdk15_timer.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/watchdog/ruuvi_nrf5_sdk15_watchdog.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/yield/ruuvi_nrf5_sdk15_yield.c \
  $(PROJ_DIR)/ruuvi.drivers.c/STMems_Standard_C_drivers/lis2dh12_STdC/driver/lis2dh12_reg.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_adc.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_advertisement.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_button.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_communication.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_flash.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_gatt.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_gpio.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_led.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_nfc.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks/ruuvi_task_sensor.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/ruuvi_driver_error.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/ruuvi_driver_sensor.c \
  $(PROJ_DIR)/ruuvi.endpoints.c/src/ruuvi_endpoints.c \
  $(PROJ_DIR)/ruuvi.endpoints.c/src/ruuvi_endpoint_3.c \
  $(PROJ_DIR)/ruuvi.endpoints.c/src/ruuvi_endpoint_5.c \
  $(PROJ_DIR)/ruuvi.endpoints.c/src/ruuvi_endpoint_6.c \
  $(PROJ_DIR)/ruuvi.endpoints.c/src/ruuvi_endpoint_8.c \
  $(PROJ_DIR)/ruuvi.endpoints.c/src/ruuvi_endpoint_fa.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/compress/ruuvi_library_compress.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/compress/liblzf-3.6/lzf_c.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/compress/liblzf-3.6/lzf_d.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/peak2peak/ruuvi_library_peak2peak.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/rms/ruuvi_library_rms.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/variance/ruuvi_library_variance.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/ringbuffer/ruuvi_library_ringbuffer.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/integration_tests/ruuvi_library_compress_test.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/integration_tests/ruuvi_library_test.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/integration_tests/ruuvi_library_test_analysis.c \
  $(PROJ_DIR)/ruuvi.libraries.c/src/integration_tests/ruuvi_library_ringbuffer_test.c
  
RUUVI_PRJ_SOURCES= \
  $(PROJ_DIR)/main.c \
  $(PROJ_DIR)/run_integration_tests.c \
  $(PROJ_DIR)/app_button.c \
  $(PROJ_DIR)/app_comms.c \
  $(PROJ_DIR)/app_dataformats.c \
  $(PROJ_DIR)/app_heartbeat.c \
  $(PROJ_DIR)/app_led.c \
  $(PROJ_DIR)/app_log.c \
  $(PROJ_DIR)/app_power.c \
  $(PROJ_DIR)/app_sensor.c

COMMON_SOURCES= \
  $(RUUVI_LIB_SOURCES) \
  $(RUUVI_PRJ_SOURCES) \
  $(NRF_LIB_SOURCES) \

COMMON_INCLUDES= \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/ble/ble_radio_notification \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis/ \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/ble_link_ctx_manager/ \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/ble/nrf_ble_scan \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/libraries/atomic_flags/ \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/bootloader \
  $(SDK_ROOT)/components/libraries/bootloader/ble_dfu/ \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/ \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/libraries/crypto \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
  $(SDK_ROOT)/components/libraries/crypto/backend/cifra \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/ \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon \
  $(SDK_ROOT)/components/libraries/crypto/backend/optiga \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/libraries/queue/ \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/serial/ \
  $(SDK_ROOT)/components/libraries/stack_info/ \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/libraries/svc \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/nfc/ndef/launchapp/ \
  $(SDK_ROOT)/components/nfc/ndef/generic/message \
  $(SDK_ROOT)/components/nfc/ndef/generic/record \
  $(SDK_ROOT)/components/nfc/ndef/parser/message \
  $(SDK_ROOT)/components/nfc/ndef/parser/record \
  $(SDK_ROOT)/components/nfc/ndef/text/ \
  $(SDK_ROOT)/components/nfc/ndef/uri \
  $(SDK_ROOT)/components/nfc/t4t_lib \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/external/mbedtls/include \
  $(SDK_ROOT)/external/nrf_tls/mbedtls/nrf_crypto/config \
  $(SDK_ROOT)/external/nrf_cc310/include \
  $(SDK_ROOT)/external/nrf_oberon/include \
  $(SDK_ROOT)/external/nrf_oberon/ \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/integration/nrfx/legacy/ \
  $(SDK_ROOT)/modules/nrfx/ \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/integration/nrfx \
  $(PROJ_DIR)/application_config \
  $(PROJ_DIR)/ruuvi.boards.c \
  $(PROJ_DIR)/ruuvi.drivers.c/BME280_driver \
  $(PROJ_DIR)/ruuvi.drivers.c/BME280_driver/selftest \
  $(PROJ_DIR)/ruuvi.drivers.c/ruuvi.dps310.c/src \
  $(PROJ_DIR)/ruuvi.drivers.c/src \
  $(PROJ_DIR)/ruuvi.drivers.c/src/integration_tests \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/acceleration \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/adc \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/atomic \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/communication \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/crypto \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/environmental \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/flash \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/gpio \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/i2c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/log \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/power \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/rtc \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/scheduler \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/spi \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/timer \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/watchdog \
  $(PROJ_DIR)/ruuvi.drivers.c/src/interfaces/yield \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/gpio \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/timer \
  $(PROJ_DIR)/ruuvi.drivers.c/src/nrf5_sdk15_platform/ruuvi.nrf_sdk15_3_overrides.c \
  $(PROJ_DIR)/ruuvi.drivers.c/src/tasks \
  $(PROJ_DIR)/ruuvi.endpoints.c/src \
  $(PROJ_DIR)/ruuvi.drivers.c/STMems_Standard_C_drivers/lis2dh12_STdC/driver \
  $(PROJ_DIR)/ruuvi.libraries.c/src \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/include \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/compress \
  $(PROJ_DIR)/ruuvi.libraries.c/src/libs/compress/liblzf-3.6 \
  $(PROJ_DIR)/ruuvi.libraries.c/src/integration_tests 

COMMON_DEFINES= \
  -DAPPLICATION_DRIVER_CONFIGURED \
  -DAPPLICATION_ENDPOINTS_CONFIGURED \
  -DMBEDTLS_CONFIG_FILE=\"nrf_crypto_mbedtls_config.h\" 
