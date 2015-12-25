/**
 * @file battery_stub.c
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

int battery_stub_energy_level_next;
int battery_stub_energy_level[16];

int battery_init(struct battery_t *battery_p,
               struct adc_device_t *dev_p,
               struct pin_device_t *pin_dev_p)
{
    return (0);
}

int battery_async_convert(struct battery_t *battery_p)
{
    return (0);
}

int battery_async_wait(struct battery_t *battery_p)
{
    return (0);
}

int battery_update(struct battery_t *battery_p)
{
    return (0);
}

int battery_get_stored_energy_level(struct battery_t *battery_p)
{
    int energy_level;

    energy_level = battery_stub_energy_level[0];


    std_printf(FSTR("battery_stub: energy_level = %d\r\n"), energy_level);
    
    return (energy_level);
}
