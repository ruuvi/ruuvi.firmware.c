#include "unity.h"

#include "app_power.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_power.h"

void setUp (void)
{
}

void tearDown (void)
{
}

void test_app_power_dc_dc_init (void)
{
    rd_status_t err_code = RD_SUCCESS;
    ri_power_regulators_t reg = {0};
#if RB_DCDC_INTERNAL_INSTALLED
    reg.DCDC_INTERNAL = 1;
#endif
#if RB_DCDC_HV_INSTALLED
    reg.DCDC_HV = 1;
#endif
    ri_power_regulators_enable_ExpectAndReturn (reg, RD_SUCCESS);
    err_code = app_dc_dc_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}