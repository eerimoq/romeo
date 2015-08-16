/**
 * @file romeo/power.h
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

#ifndef __ROMEO_POWER_H__
#define __ROMEO_POWER_H__

#include "simba.h"
#include "romeo.h"

#define POWER_STORED_ENERGY_LEVEL_MIN   0
#define POWER_STORED_ENERGY_LEVEL_MAX 100

struct power_t {
    struct adc_driver_t adc;
    struct {
        int samples[1];
    } ongoing;
    struct {
        int samples[1];
        int stored_energy_level;
    } updated;
};

/**
 * Initialize power object.
 * @param[out] power_p Object to initialize.
 * @return zero(0) or negative error code
 */
int power_init(struct power_t *power_p,
               struct adc_device_t *dev_p,
               struct pin_device_t *pin_dev_p);

/**
 * Start an asynchronous convertion of the power level. Call
 * power_async_wait() to save the converted value in this object.
 * @param[in] power_p Initialized power object.
 * @return zero(0) or negative error code
 */
int power_async_convert(struct power_t *power_p);

/**
 * Wait for the asynchronous convertion to finish.
 * @param[in] power_p Initialized power object.
 * @return zero(0) or negative error code
 */
int power_async_wait(struct power_t *power_p);

/**
 * Update the object from the latest coverted samples.
 * @param[in] power_p Initialized power object.
 * @return zero(0) or negative error code.
 */
int power_update(struct power_t *power_p);

/**
 * Get the value of the stored energy level.
 * @param[in] power_p Initialized power object.
 * @return Linerar scale from 0 to 100, where 0 means no energy stored
 *         and 100 means that the maximum amount of energy is stored.
 *         Otherwise negative error code.
 */
int power_get_stored_energy_level(struct power_t *power_p);

#endif
