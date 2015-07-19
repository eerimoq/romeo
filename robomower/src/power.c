/**
 * @file power.c
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

#include "simba.h"
#include "robomower.h"

int power_init(struct power_t *power_p,
               struct adc_device_t *dev_p,
               struct pin_device_t *pin_dev_p)
{
    adc_init(&power_p->adc,
             dev_p,
             pin_dev_p,             
             ADC_REFERENCE_VCC,
             1000);

    return (0);
}

int power_get_stored_energy_level(struct power_t *power_p)
{
    int samples[1];

    adc_convert(&power_p->adc, samples, membersof(samples));

    if (samples[0] > 1000) {
        samples[0] = 1000;
    }

    return (samples[0] / 10);
}
