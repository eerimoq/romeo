/**
 * @file robomower/perimiter_wire_rx.c
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

/* The coefficients used as a reference in the matched filter. */
static int8_t coefficients[] = {
    1, 1, -1, -1, 1, -1, 1, -1,
    -1, 1, -1, 1, 1, -1, -1, 1,
    -1, -1, 1, -1, -1, 1, 1, -1
};

int perimiter_wire_rx_init(struct perimiter_wire_rx_t *pwire_p,
                           struct pin_device_t *pin_dev_in1_p,
                           struct pin_device_t *pin_dev_in2_p)
{
    /* adc_init(&pwire_p->adc.drv, */
    /*          1000, */
    /*          pwire_p->adc.buf, */
    /*          membersof(pwire_p->adc.buf), */
    /*          ADC_MODE_CIRCULAR); */
    return (0);
}

int perimiter_wire_rx_start(struct perimiter_wire_rx_t *pwire_p)
{
    /* adc_start(&pwire_p->adc.drv); */

    return (0);
}
