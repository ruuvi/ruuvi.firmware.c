/**
 * @file app_testing.h
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 * @author DG12 <DGerman@eal-World-Systems.com>
 * @date 2021-10-22
 *
 * @brief A set of utilities to facilitate testing.
 */

/* @brief TESTABLE_STATIC: An inline macro to use with the Ceedling unit test.
 * See https://www.design-reuse.com/articles/47812/embedded-software-unit-testing-with-ceedling.html
 *        for additional information.
 * and  ruuvi.drivers.c/test/interfaces/environmental/test_ruuvi_interface_shtcx.c
 *      ruuvi.drivers.c/src/tasks/ruuvi_task_led.c
 *
 * 1) example usage:    TESTABLE_STATIC rd_status_t funky (const uint8_t * const astring, uint8_t along)
 *
 * When compiled for use for the Ceedling test this generates:
 *       rd_status_t funky (const uint8_t * const astring, uint8_t along)
 *
 * When compiled normally:
 *        static rd_status_t funky (const uint8_t * const astring, uint8_t along)
 *
 * 2)example usage:    TESTABLE_STATIC ri_timer_id_t m_timer;
 *
 * When compiled for use with the Ceedling test this generates:
 *      ri_timer_id_t m_timer;
 *
 * When compiled normally:
 *      static ri_timer_id_t m_timer;
 */
#ifdef CEEDLING
#   define TESTABLE_STATIC          //  generate nothing
#else
#   define TESTABLE_STATIC static   // generate: static
#endif

