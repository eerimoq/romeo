/**
 * @file base_station.c
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

FS_COUNTER_DEFINE("/base_station/counters/tick", base_station_tick);

int base_station_init(struct base_station_t *base_station_p)
{
    perimeter_wire_tx_init(&base_station_p->perimeter,
                           &pin_d7_dev,
                           &pin_d8_dev,
                           &adc_0_dev,
                           &pin_a0_dev);

    return (0);
}

int base_station_start(struct base_station_t *base_station_p)
{
    perimeter_wire_tx_start(&base_station_p->perimeter);

    return (0);
}

int base_station_stop(struct base_station_t *base_station_p)
{
    //perimeter_wire_tx_stop(&base_station_p->perimeter);

    return (0);
}

int base_station_tick(struct base_station_t *base_station_p)
{
    FS_COUNTER_INC(base_station_tick, 1);

    perimeter_wire_tx_get_current(&base_station_p->perimeter);

    return (0);
}
