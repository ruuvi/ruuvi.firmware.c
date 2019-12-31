/**
 * @file test_task_led.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2019-11-18
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "unity.h"
#include "ruuvi_boards.h"
#include "mock_ruuvi_driver_error.h"
#include "mock_ruuvi_interface_gpio.h"
#include "mock_ruuvi_interface_log.h"
#include "task_led.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
static const ruuvi_interface_gpio_id_t leds[RUUVI_BOARD_LEDS_NUMBER] =
    RUUVI_BOARD_LEDS_LIST;
#pragma GCC diagnostic pop

void setUp (void)
{
    ruuvi_interface_gpio_is_init_ExpectAndReturn (true);

    for (int ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        ruuvi_interface_gpio_configure_ExpectAndReturn (leds[ii],
                RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE,
                RUUVI_DRIVER_SUCCESS);
        ruuvi_interface_gpio_write_ExpectAndReturn (leds[ii], !RUUVI_BOARD_LEDS_ACTIVE_STATE,
                RUUVI_DRIVER_SUCCESS);
    }

    task_led_init();
    uint16_t led = task_led_activity_led_get();
    TEST_ASSERT (RUUVI_INTERFACE_GPIO_ID_UNUSED == led);
}

void tearDown (void)
{
    for (int ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        ruuvi_interface_gpio_configure_ExpectAndReturn (leds[ii],
                RUUVI_INTERFACE_GPIO_MODE_HIGH_Z,
                RUUVI_DRIVER_SUCCESS);
    }

    task_led_uninit();
    uint16_t led = task_led_activity_led_get();
    TEST_ASSERT (RUUVI_INTERFACE_GPIO_ID_UNUSED == led);
}

/**
 * @brief LED initialization function.
 * - Returns error if leds were already initialized.
 * - Initializes GPIO if GPIO wasn't initialized.
 * - returns error code if GPIO cannot be initialized
 * - Configures GPIOs as high-drive output and sets LEDs as inactive.
 * - Sets activity led to uninitialized
 *
 * @retval RUUVI_DRIVER_SUCCESS if no errors occured.
 * @retval RUUVI_DRIVER_ERROR_INVALID_STATE if leds were already initialized.
 * @retval error code from stack on other error.
 **/
/* Case: Success, GPIO was initialzed */
void test_task_led_init_leds_success_gpio_was_init (void)
{
    tearDown();
    ruuvi_interface_gpio_is_init_ExpectAndReturn (true);

    for (int ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        ruuvi_interface_gpio_configure_ExpectAndReturn (leds[ii],
                RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE,
                RUUVI_DRIVER_SUCCESS);
        ruuvi_interface_gpio_write_ExpectAndReturn (leds[ii], !RUUVI_BOARD_LEDS_ACTIVE_STATE,
                RUUVI_DRIVER_SUCCESS);
    }

    task_led_init();
    uint16_t led = task_led_activity_led_get();
    TEST_ASSERT (RUUVI_INTERFACE_GPIO_ID_UNUSED == led);
}

/* Case: Success, GPIO was not initialzed */
void test_task_led_init_leds_gpio_not_init (void)
{
    tearDown();
    ruuvi_interface_gpio_is_init_ExpectAndReturn (false);
    ruuvi_interface_gpio_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);

    for (int ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        ruuvi_interface_gpio_configure_ExpectAndReturn (leds[ii],
                RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE,
                RUUVI_DRIVER_SUCCESS);
        ruuvi_interface_gpio_write_ExpectAndReturn (leds[ii], !RUUVI_BOARD_LEDS_ACTIVE_STATE,
                RUUVI_DRIVER_SUCCESS);
    }

    task_led_init();
    uint16_t aled = task_led_activity_led_get();
    TEST_ASSERT (RUUVI_INTERFACE_GPIO_ID_UNUSED == aled);
}

/* Case: fail on second init, GPIO was not initialzed */
void test_task_led_init_leds_twice_fails (void)
{
    tearDown();
    ruuvi_interface_gpio_is_init_ExpectAndReturn (false);
    ruuvi_interface_gpio_init_ExpectAndReturn (RUUVI_DRIVER_SUCCESS);

    for (int ii = 0; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        ruuvi_interface_gpio_configure_ExpectAndReturn (leds[ii],
                RUUVI_INTERFACE_GPIO_MODE_OUTPUT_HIGHDRIVE,
                RUUVI_DRIVER_SUCCESS);
        ruuvi_interface_gpio_write_ExpectAndReturn (leds[ii], !RUUVI_BOARD_LEDS_ACTIVE_STATE,
                RUUVI_DRIVER_SUCCESS);
    }

    task_led_init();
    ruuvi_driver_status_t error = task_led_init();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == error);
}

/* Case: GPIO was not initialzed, GPIO init fails */
void test_task_led_init_leds_gpio_fails (void)
{
    tearDown();
    ruuvi_interface_gpio_is_init_ExpectAndReturn (false);
    ruuvi_interface_gpio_init_ExpectAndReturn (RUUVI_DRIVER_ERROR_INTERNAL);
    ruuvi_driver_status_t error = task_led_init();
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INTERNAL == error);
}

/**
 * @brief LED write function. Set given LED ON or OFF.
 *
 * @param[in] led  LED to change, use constant from RUUVI_BOARDS
 * @param[in] state  true to turn led on, false to turn led off.
 *
 * @retval @c RUUVI_DRIVER_SUCCESS if value was written
 * @retval @c RUUVI_ERROR_INVALID_PARAM  if GPIO pin is not led.
 * @retval @c RUUVI_DRIVER_ERROR_INVALID_STATE if GPIO task is not initialized.
 **/
void test_task_led_write_not_init()
{
    tearDown();
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= task_led_write (RUUVI_BOARD_LED_1, RUUVI_BOARD_LEDS_ACTIVE_STATE);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_STATE == err_code);
}

void test_task_led_write_not_led()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= task_led_write (RUUVI_INTERFACE_GPIO_ID_UNUSED,
                                RUUVI_BOARD_LEDS_ACTIVE_STATE);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_PARAM == err_code);
}

void test_task_led_write_valid()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    const ruuvi_interface_gpio_id_t id = {.pin = RUUVI_BOARD_LED_1};
    ruuvi_interface_gpio_write_ExpectAndReturn (id,
            RUUVI_BOARD_LEDS_ACTIVE_STATE,
            RUUVI_DRIVER_SUCCESS);
    err_code |= task_led_write (RUUVI_BOARD_LED_1, true);
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
}

/**
 * @brief Set LED which is used to indicate activity.
 *
 * This function can be called before GPIO or LEDs are initialized.
 * Call with RUUVI_INTERFACE_GPIO_ID_UNUSED to disable activity indication.
 *
 * @param[in] led LED to indicate activity with.
 *
 * @retval RUUVI_DRIVER_SUCCESS if valid led was set.
 * @retval RUUVI_DRIVER_ERROR_INVALID_PARAM if there is no pin in LED.
 */
void test_task_led_activity_led_set_valid ()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    const ruuvi_interface_gpio_id_t id = {.pin = RUUVI_BOARD_LED_1};
    ruuvi_interface_gpio_write_ExpectAndReturn (id,
            RUUVI_BOARD_LEDS_ACTIVE_STATE,
            RUUVI_DRIVER_SUCCESS);
    ruuvi_interface_gpio_write_ExpectAndReturn (id,
            !RUUVI_BOARD_LEDS_ACTIVE_STATE,
            RUUVI_DRIVER_SUCCESS);
    err_code |= task_led_activity_led_set (RUUVI_BOARD_LED_1);
    task_led_activity_indicate (true);
    task_led_activity_indicate (false);
    uint16_t led = task_led_activity_led_get();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (RUUVI_BOARD_LED_1 == led);
}

void test_task_led_activity_led_set_unused ()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= task_led_activity_led_set (RUUVI_INTERFACE_GPIO_ID_UNUSED);
    task_led_activity_indicate (true);
    task_led_activity_indicate (false);
    uint16_t led = task_led_activity_led_get();
    TEST_ASSERT (RUUVI_DRIVER_SUCCESS == err_code);
    TEST_ASSERT (RUUVI_INTERFACE_GPIO_ID_UNUSED == led);
}

void test_task_led_activity_led_set_invalid ()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    err_code |= task_led_activity_led_set (2312);
    TEST_ASSERT (RUUVI_DRIVER_ERROR_INVALID_PARAM == err_code);
}

void test_task_led_is_led ()
{
    ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
    bool valid_led = false;

    for (size_t ii = 0U; ii < RUUVI_BOARD_LEDS_NUMBER; ii++)
    {
        TEST_ASSERT (is_led (leds[ii].pin));
    }

    TEST_ASSERT (!is_led (RUUVI_BOARD_PIN_UNUSED));
}