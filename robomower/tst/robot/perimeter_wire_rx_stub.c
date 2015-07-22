/**
 * @file robomower/perimeter_wire_rx_stub.c
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

int perimeter_wire_rx_init(struct perimeter_wire_rx_t *perimeter_wire_p,
                           struct adc_device_t *dev_p,
                           struct pin_device_t *pin_dev_p)
{
    return (0);
}

int perimeter_wire_rx_start(struct perimeter_wire_rx_t *perimeter_wire_p)
{
    return (0);
}

float perimeter_wire_rx_get_signal(struct perimeter_wire_rx_t *perimeter_wire_p)
{
    const struct testdata_t FAR *data_p = &testdata_p[data_index++];

    if (data_p->energy_level != -1) {
        std_printk(STD_LOG_NOTICE,
                   FSTR("perimeter_wire_rx_stub: perimeter_signal = %d"),
                   (int)data_p->perimeter_signal);
        return (data_p->perimeter_signal);
    } else {
        return (0.0f);
    }
}
