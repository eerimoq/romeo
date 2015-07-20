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

#define FLT_MIN -1000000.0f
#define FLT_MAX 1000000.0f

#define THRESHOLD_MAX 10.0f
#define THRESHOLD_MIN -10.0f

#define COEFFICIENTS_MAX 24
#define INPUT_MAX (2 * PERIMETER_WIRE_RX_SAMPLES_MAX)
#define OUTPUT_MAX (INPUT_MAX - COEFFICIENTS_MAX + 1)

/* The coefficients used as a reference in the matched filter. */
static float coefficients[COEFFICIENTS_MAX] = {
    1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f
};

int perimeter_wire_rx_init(struct perimeter_wire_rx_t *perimeter_wire_p,
                           struct adc_device_t *dev_p,
                           struct pin_device_t *pin_dev_p)
{
    perimeter_wire_p->signal = 0.0f;

    adc_init(&perimeter_wire_p->adc,
             dev_p,
             pin_dev_p,
             ADC_REFERENCE_VCC,
             962);

    return (0);
}

int perimeter_wire_rx_start(struct perimeter_wire_rx_t *perimeter_wire_p)
{
    /* Start first asynchronous convertion. */
    adc_async_convert(&perimeter_wire_p->adc,
                      perimeter_wire_p->samples,
                      membersof(perimeter_wire_p->samples));

    return (0);
}

int perimeter_wire_rx_get_signal(struct perimeter_wire_rx_t *perimeter_wire_p,
                                 float *value)
{
    int i, i_mod;
    float input[INPUT_MAX];
    float output[OUTPUT_MAX];
    float min, max, signal;

    /* Wait for ongoing asynchronous convertion to finish. */
    if (!adc_async_wait(&perimeter_wire_p->adc)) {
        std_printk(STD_LOG_WARNING, FSTR("convertion has not finished"));
    }

    /* Copy samples to filter input buffer. */
    for (i = 0; i < INPUT_MAX; i++) {
        i_mod = (i % PERIMETER_WIRE_RX_SAMPLES_MAX);
        input[i] = (float)(perimeter_wire_p->samples[i_mod] - 512) / 512.0f;
    }

    /* Start next asynchronous convertion. */
    adc_async_convert(&perimeter_wire_p->adc,
                      perimeter_wire_p->samples,
                      membersof(perimeter_wire_p->samples));

    filter_firf(input,
                membersof(input),
                coefficients,
                membersof(coefficients),
                output);

    /* Get min and max values. */
    min = FLT_MAX;
    max = FLT_MIN;
    for (i = 0; i < OUTPUT_MAX; i++) {
        if (output[i] > max) {
            max = output[i];
        }
        if (output[i] < min) {
            min = output[i];
        }
    }

    if (max > -min) {
        signal = max;
    } else {
        signal = min;
    }

    /* Low pass filtering of the signal. */
    signal = (4.0f * perimeter_wire_p->signal + 1.0f * signal) / 5.0f;

    perimeter_wire_p->signal = signal;

    /* Check versus thresholds. */
    if ((signal < THRESHOLD_MAX) && (signal > THRESHOLD_MIN)) {
        return (-1);
    }

    *value = signal;

    return (0);
}
