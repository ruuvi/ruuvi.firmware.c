#include "ruuvi_error.h"
#include "ruuvi_sensor.h"
#include "acceleration.h"
#include "environmental.h"
#include <stddef.h>

ruuvi_status_t task_peripherals_init(void);
ruuvi_status_t task_leds_blink(uint32_t time_per_led);
ruuvi_status_t task_nfc_process(void);

ruuvi_status_t task_accelerometer_init (void);
ruuvi_status_t task_accelerometer_setup(const ruuvi_sensor_configuration_t* configuration);
ruuvi_status_t task_accelerometer_get  (ruuvi_acceleration_data_t* data);

ruuvi_status_t task_environmental_init (void);
ruuvi_status_t task_environmental_setup(const ruuvi_sensor_configuration_t* configuration);
ruuvi_status_t task_environmental_get  (ruuvi_environmental_data_t* data);

ruuvi_status_t task_bluetooth_init(void);
ruuvi_status_t task_bluetooth_advertise(uint8_t* data, size_t data_length);
ruuvi_status_t task_bluetooth_send_asynchronous(uint8_t* data, size_t data_length);
ruuvi_status_t task_bluetooth_process(void);

ruuvi_status_t task_generate_keys(void);
ruuvi_status_t task_generate_dummy_hash(void);
ruuvi_status_t task_sha256_message(uint8_t* message, size_t message_size);
ruuvi_status_t task_sign_hash(void);
ruuvi_status_t task_verify_hash(void);
ruuvi_status_t task_pop_process_incoming_data(uint8_t* message, size_t message_length);
ruuvi_status_t task_pop_send_challenge(void);
ruuvi_status_t task_pop_reset_state(void);
ruuvi_status_t task_pop_send_challenge_status(ruuvi_status_t challenge_status);