/**
 * @file robomower/power.h
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
 * This file is part of the RoboMower project.
 */

#ifndef __ROBOMOWER_POWER_H__
#define __ROBOMOWER_POWER_H__

#include "simba.h"
#include "robomower.h"

#define POWER_STORED_ENERGY_LEVEL_MIN   0
#define POWER_STORED_ENERGY_LEVEL_MAX 100

struct power_t {
    struct adc_driver_t adc;
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
 * Get current stored energy level in the power supply.
 * @param[in] power_p Object to initialize.
 * @return Linerar scale from 0 to 100, where 0 means no energy stored
 *         and 100 means that the maximum amount of energy is stored.
 *         Otherwise negative error code.
 */
int power_get_stored_energy_level(struct power_t *power_p);

#endif
