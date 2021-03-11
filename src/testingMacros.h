/* @file testingmacros.h
 * @author Dennis German<DGerman@Real-World-Systems.com>
 * @date 2021-03-03
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.

 * @brief testing macros
 * Various defines and macros for easy coding of tests */

/* @brief
 * When testing with the Ceedling packsge symbols must not be declared static since
 *  they would not be not external
 Example:    staticNC app_log_record_t m_log_input_block;
             staticNC void heartbeat ... */

#ifndef CEEDLING
#define staticNC  static
#else
#define staticNC
#endif

/* @brief status makes sense if pass or fail rather than true or false */

#define PASS false
#define FAIL !PASS

/* @brief  creating JSON is ackward with many quotes. It's easier to use these */

#define Joutput_keyword(k)  printfp ("\"k\":");   //  example joutput_keyword(record_set)  N.B. no quotes on invocation
#define Joutput_status      if (status == PASS) { printfp ("\"pass\",\r\n"); } else { printfp ("\"fail\",\r\n"); }
#define Joutput_close       printfp ("},\r\n");


#define Joutput_init        printfp ("\"init\":");
#define Joutput_uninit      printfp ("\"uninit\":");

/** @brief function pointer to output test information */
typedef void (*rd_test_print_fp) (const char * const msg);
