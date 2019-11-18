#include "ruuvi_library_test.h"
#include "test_library.h"


#if RUUVI_RUN_TESTS
static void print_test (const char * const msg)
{
    ruuvi_interface_log (RUUVI_INTERFACE_LOG_INFO, msg);
}

void test_library_run (void)
{
    print_test ("'library':{\r\n");
    ruuvi_library_test_all_run (print_test);
    print_test ("}\r\n");
}
#else
// Dummy implementation
ruuvi_driver_status_t test_library_status (size_t * total, size_t * passed)
{
    *total = 0;
    *passed = 0;
    return RUUVI_DRIVER_SUCCESS;
}
void test_library_run (void)
{}
#endif