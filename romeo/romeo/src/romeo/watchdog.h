/**
 * @file romeo/watchdog.h
 * @version 0.1
 *
 * @section License
 * Copyright (C) 2015, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Romeo project.
 */

#ifndef __ROMEO_WATCHDOG_H__
#define __ROMEO_WATCHDOG_H__

#include "simba.h"
#include "romeo.h"

struct watchdog_t {
    int started;
    int timeout;
    int count;
};

/**
 * Initialize watchdog object.
 * @param[out] watchdog_p Object to initialize.
 * @param[in] timeout Watchdog timeout in number of ticks.
 * @return zero(0) or negative error code
 */
int watchdog_init(struct watchdog_t *watchdog_p,
                  int timeout);

/**
 * Start watchdog object.
 * @param[in] watchdog_p Initialized watchdog object.
 * @return zero(0) or negative error code
 */
int watchdog_start(struct watchdog_t *watchdog_p);

/**
 * Start watchdog object.
 * @param[in] watchdog_p Initialized watchdog object.
 * @return zero(0) or negative error code
 */
int watchdog_stop(struct watchdog_t *watchdog_p);

/**
 * Call once per tick to decremnet the time until it expires.
 * @param[in] watchdog_p Initialized watchdog object.
 * @return Number of ticks left until it expires.
 */
int watchdog_tick(struct watchdog_t *watchdog_p);

/**
 * Reset the timeout to the initial value.
 * @param[in] watchdog_p Initialized watchdog object.
 * @return zero(0) or negative error code
 */
int watchdog_kick(struct watchdog_t *watchdog_p);

#endif
