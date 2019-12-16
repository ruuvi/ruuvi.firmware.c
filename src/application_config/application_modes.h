/**
 * Select application mode definitions to load.
 * Ideally the desired mode is passed as a makefile flag.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef APPLICATION_MODES_H
#define APPLICATION_MODES_H
#ifdef APPLICATION_MODE_LONGLIFE
#include "application_mode_longlife.h"
#elif APPLICATION_MODE_LONGMEM
#include "application_mode_longmem.h"
#elif DEBUG
#include "application_mode_debug.h"
#endif
#include "application_mode_default.h"
#endif