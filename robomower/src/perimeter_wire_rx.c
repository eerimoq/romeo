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

#define COEFFICIENTS_MAX 24
#define INPUT_MAX PERIMETER_WIRE_RX_SAMPLES_MAX
#define OUTPUT_MAX (INPUT_MAX - COEFFICIENTS_MAX + 1)

/* The coefficients used as a reference in the matched filter. */
static float coefficients[COEFFICIENTS_MAX] = {
    1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f
};

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
    /* Start first asynchronous convertion. */
    adc_async_convert(&pwire_p->adc,
                      pwire_p->samples,
                      membersof(pwire_p->samples));

    return (0);
}

float perimeter_wire_rx_get_signal(struct perimeter_wire_rx_t *pwire_p)
{
    int i;
    float input[INPUT_MAX];
    float output[OUTPUT_MAX];

    /* Wait for ongoing asynchronous convertion to finish. */
    if (!adc_async_wait(&pwire_p->adc)) {
        std_printk(STD_LOG_WARNING, FSTR("convertion has not finished"));
    }

    /* Copy samples to filter input buffer. */
    for (i = 0; i < INPUT_MAX; i++) {
        input[i] = (pwire_p->samples[i] - 512);
    }

    /* Start next asynchronous convertion. */
    adc_async_convert(&pwire_p->adc,
                      pwire_p->samples,
                      membersof(pwire_p->samples));

    filter_firf(input,
                membersof(input),
                coefficients,
                membersof(coefficients),
                output);

    /* TODO: should be calculated from output array. */
    return (input[0]);
}
