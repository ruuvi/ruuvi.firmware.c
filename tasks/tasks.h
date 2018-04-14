#include "ruuvi_error.h"
#include "ruuvi_sensor.h"
#include "acceleration.h"
#include "environmental.h"
#include <stddef.h>

ruuvi_status_t task_init_peripherals(void);
ruuvi_status_t task_blink_leds(uint32_t time_per_led);
ruuvi_status_t task_nfc_process(void);

ruuvi_status_t task_init_accelerometer (void);
ruuvi_status_t task_setup_accelerometer(void);
ruuvi_status_t task_get_acceleration   (ruuvi_acceleration_data_t* data);

ruuvi_status_t task_init_environmental (void);
ruuvi_status_t task_setup_environmental(void);
ruuvi_status_t task_get_environmental  (ruuvi_environmental_data_t* data);

ruuvi_status_t task_bluetooth_init(void);
ruuvi_status_t task_bluetooth_advertise(uint8_t* data, size_t data_length);
ruuvi_status_t task_bluetooth_send_asynchronous(uint8_t* data, size_t data_length);
ruuvi_status_t task_bluetooth_process(void);

ruuvi_status_t task_generate_keys(void);
ruuvi_status_t task_generate_dummy_hash(void);
ruuvi_status_t task_sha256_message(uint8_t* message, size_t message_size);
ruuvi_status_t task_sign_hash(void);
ruuvi_status_t task_verify_hash(void);
ruuvi_status_t task_process_incoming_data(uint8_t* message, size_t message_length);
ruuvi_status_t task_pop_send_challenge(void);