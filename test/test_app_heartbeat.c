#include "unity.h"

#include "app_config.h"
#include "app_heartbeat.h"
#include "mock_app_comms.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_scheduler.h"
#include "mock_ruuvi_interface_timer.h"

#include "application_mode_default.h" //!< Ceedling doesn't follow includes by default.

static unsigned int mock_tid = 0xAA; //!< Mock timer ID to be returned, size system int.
static void * p_mock_tid = &mock_tid; //!< Pointer to mock ID.

void setUp (void)
{
    ri_log_init_IgnoreAndReturn (RD_SUCCESS);
    ri_log_Ignore();
    rd_error_check_Ignore();
}

void tearDown (void)
{
}


/**
 * @brief Initializes timers for reading and sending heartbeat transmissions.
 *
 * The heartbeat interval should be at most as logging rate to make sure
 * that application will log fresh data.
 *
 * @retval RD_SUCCESS on success
 * @retval RD_ERROR_INVALID_STATE if timers or scheduler is not initialized.
 * @retval RD_ERROR_RESOURCES if a timer cannot be allocated.
 */
void test_app_heartbeat_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_id_t * p_heart_timer = get_heart_timer();
    ri_scheduler_is_init_ExpectAndReturn (true);
    ri_timer_is_init_ExpectAndReturn (true);
    ri_timer_create_ExpectAndReturn (p_heart_timer, RI_TIMER_MODE_REPEATED,
                                     &schedule_heartbeat_isr, RD_SUCCESS);
    ri_timer_create_ReturnArrayThruPtr_p_timer_id (&p_mock_tid, 1);
    ri_timer_start_ExpectAndReturn (&mock_tid, APP_HEARTBEAT_INTERVAL_MS, NULL, RD_SUCCESS);
    err_code = app_heartbeat_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_heartbeat_init_notimer (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_scheduler_is_init_ExpectAndReturn (true);
    ri_timer_is_init_ExpectAndReturn (false);
    err_code = app_heartbeat_init();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
}

void test_app_heartbeat_init_noscheduler (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_scheduler_is_init_ExpectAndReturn (false);
    err_code = app_heartbeat_init();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
}

void test_app_heartbeat_init_timer_alloc_fail (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_timer_id_t * p_heart_timer = get_heart_timer();
    ri_scheduler_is_init_ExpectAndReturn (true);
    ri_timer_is_init_ExpectAndReturn (true);
    ri_timer_create_ExpectAndReturn (p_heart_timer, RI_TIMER_MODE_REPEATED,
                                     &schedule_heartbeat_isr, RD_ERROR_RESOURCES);
    err_code = app_heartbeat_init();
    TEST_ASSERT (RD_ERROR_RESOURCES == err_code);
}


void test_schedule_heartbeat_isr (void)
{
    ri_scheduler_event_put_ExpectAndReturn (NULL, 0, &heartbeat, RD_SUCCESS);
}

void test_heartbeat_ok (void)
{
    
}