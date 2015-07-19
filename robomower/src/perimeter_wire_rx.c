/**
 * @file robomower/perimeter_wire_rx.c
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

#if 0
/* The coefficients used as a reference in the matched filter. */
static int8_t coefficients[] = {
    1, 1, -1, -1, 1, -1, 1, -1,
    -1, 1, -1, 1, 1, -1, -1, 1,
    -1, -1, 1, -1, -1, 1, 1, -1
};
#endif

int perimeter_wire_rx_init(struct perimeter_wire_rx_t *pwire_p,
                           struct adc_device_t *dev_p,
                           struct pin_device_t *pin_dev_p)
{
    adc_init(&pwire_p->adc,
             dev_p,
             pin_dev_p,             
             ADC_REFERENCE_VCC,
             1000);

    return (0);
}

int perimeter_wire_rx_start(struct perimeter_wire_rx_t *pwire_p)
{
    return (0);
}

float perimeter_wire_rx_get_signal(struct perimeter_wire_rx_t *pwire_p)
{
    int samples[1];

    adc_convert(&pwire_p->adc, samples, membersof(samples));
    
    return (samples[0] - 512);
}
