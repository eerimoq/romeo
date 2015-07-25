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
 * This file is part of the Romeo project.
 */

#include "simba.h"
#include "romeo.h"
#include "testdata.h"

int power_stub_energy_level_next;
int power_stub_energy_level[16];

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
    int energy_level;

    energy_level = power_stub_energy_level[0];


    std_printk(STD_LOG_NOTICE,
               FSTR("power_stub: energy_level = %d"),
               energy_level);
    
    return (energy_level);
}
