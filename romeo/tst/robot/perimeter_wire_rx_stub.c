/**
 * @file romeo/perimeter_wire_rx_stub.c
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

int perimeter_wire_rx_stub_signal_next;
float perimeter_wire_rx_stub_signal[16];

int perimeter_wire_rx_init(struct perimeter_wire_rx_t *perimeter_wire_p,
                           struct adc_device_t *dev_p,
                           struct pin_device_t *pin_dev_p)
{
    return (0);
}

int perimeter_wire_rx_async_convert(struct perimeter_wire_rx_t *perimeter_wire_p)
{
    return (0);
}

int perimeter_wire_rx_async_wait(struct perimeter_wire_rx_t *perimeter_wire_p)
{
    return (0);
}

int perimeter_wire_rx_update(struct perimeter_wire_rx_t *perimeter_wire_p)
{
    return (0);
}

float perimeter_wire_rx_get_signal(struct perimeter_wire_rx_t *perimeter_wire_p)
{
    float signal;

    signal = perimeter_wire_rx_stub_signal[0];

    std_printk(STD_LOG_NOTICE,
               FSTR("perimeter_wire_rx_stub: perimeter_signal = %d"),
               (int)signal);

    return (signal);
}
