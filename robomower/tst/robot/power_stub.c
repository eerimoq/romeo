/**
 * @file power_stub.c
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
#include "testdata.h"

static int data_index = 0;

int power_init(struct power_t *power_p,
               struct adc_device_t *dev_p,
               struct pin_device_t *pin_dev_p)
{
    return (0);
}

int power_start(struct power_t *power_p)
{
    return (0);
}

int power_get_stored_energy_level(struct power_t *power_p)
{
    const struct testdata_t FAR *data_p;

    do {
        data_p = &testdata_p[data_index++];
    } while (data_p->energy_level == -2);

    if (data_p->energy_level != -1) {
        std_printk(STD_LOG_NOTICE,
                   FSTR("power_stub: energy_level = %d"),
                   data_p->energy_level);
        return (data_p->energy_level);
    } else {
        return (0);
    }
}
