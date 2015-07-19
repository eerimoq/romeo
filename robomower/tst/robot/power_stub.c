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

/* Those are defined and set in main.c.*/
extern int power_testdata_index;
extern int power_testdata_max;
extern const int8_t FAR *power_testdata_stored_energy_level;

int power_init(struct power_t *power_p)
{
    return (0);
}

int power_get_stored_energy_level(struct power_t *power_p)
{
    if (power_testdata_index < power_testdata_max) {
        return (power_testdata_stored_energy_level[power_testdata_index++]);
    } else {
        return (0);
    }
}
