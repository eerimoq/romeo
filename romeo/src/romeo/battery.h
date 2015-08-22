/**
 * @file romeo/battery.h
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

struct battery_t {
    struct adc_driver_t adc;
    float battery_voltage_full;
    struct {
        int samples[1];
    } ongoing;
    struct {
        int samples[1];
        float battery_voltage;
        int stored_energy_level;
    } updated;
};

/**
 * Initialize battery object.
 * @param[out] battery_p Object to initialize.
 * @return zero(0) or negative error code
 */
int battery_init(struct battery_t *battery_p,
               struct adc_device_t *dev_p,
               struct pin_device_t *pin_dev_p);

/**
 * Start an asynchronous convertion of the battery level. Call
 * battery_async_wait() to save the converted value in this object.
 * @param[in] battery_p Initialized battery object.
 * @return zero(0) or negative error code
 */
int battery_async_convert(struct battery_t *battery_p);

/**
 * Wait for the asynchronous convertion to finish.
 * @param[in] battery_p Initialized battery object.
 * @return zero(0) or negative error code
 */
int battery_async_wait(struct battery_t *battery_p);

/**
 * Update the object from the latest coverted samples.
 * @param[in] battery_p Initialized battery object.
 * @return zero(0) or negative error code.
 */
int battery_update(struct battery_t *battery_p);

/**
 * Get the value of the stored energy level.
 * @param[in] battery_p Initialized battery object.
 * @return Linerar scale from 0 to 100, where 0 means no energy stored
 *         and 100 means that the maximum amount of energy is stored.
 *         Otherwise negative error code.
 */
int battery_get_stored_energy_level(struct battery_t *battery_p);

/**
 * Get the value of the current battery voltage.
 * @param[in] battery_p Initialized battery object.
 * @return Battery voltage.
 */
float battery_get_battery_voltage(struct battery_t *battery_p);

#endif
