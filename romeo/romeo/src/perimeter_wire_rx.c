/**
 * @file romeo/perimeter_wire_rx.c
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
#include <limits.h>

#define COEFFICIENTS_MAX 96
#define NUMBER_OF_NON_ZERO_COEFFICIENTS 64
#define INPUT_MAX (2 * PERIMETER_WIRE_RX_SAMPLES_MAX)
#define OUTPUT_MAX (INPUT_MAX - COEFFICIENTS_MAX + 1)

/* The coefficients used as a reference in the matched filter. */
static int coefficients[COEFFICIENTS_MAX] = {
    1,  1,  1,  1,  0,  0,  0,  0,
    -1, -1, -1, -1, -0, -0, -0, -0,
    1,  1,  1,  1, -1, -1, -1, -1,
    1,  1,  1,  1, -1, -1, -1, -1,
    -0, -0, -0, -0,  1,  1,  1,  1,
    -1, -1, -1, -1,  1,  1,  1,  1,
    0,  0,  0,  0, -1, -1, -1, -1,
    -0, -0, -0, -0,  1,  1,  1,  1,
    -1, -1, -1, -1,  0,  0,  0,  0,
    1,  1,  1,  1, -1, -1, -1, -1,
    -0, -0, -0, -0,  1,  1,  1,  1,
    0,  0,  0,  0, -1, -1, -1, -1
};

int perimeter_wire_rx_init(struct perimeter_wire_rx_t *perimeter_p,
                           struct adc_device_t *dev_p,
                           struct pin_device_t *pin_dev_p)
{
    perimeter_p->updated.signal = 0.0f;
    perimeter_p->updated.quality = 0.0f;
    perimeter_p->filter_weight = 0.0f;

    adc_init(&perimeter_p->adc,
             dev_p,
             pin_dev_p,
             ADC_REFERENCE_VCC,
             38462);

    return (0);
}

int perimeter_wire_rx_async_convert(struct perimeter_wire_rx_t *perimeter_p)
{
    /* Start first asynchronous convertion. */
    return (adc_async_convert(&perimeter_p->adc,
                              perimeter_p->ongoing.samples,
                              membersof(perimeter_p->ongoing.samples)));
}

int perimeter_wire_rx_async_wait(struct perimeter_wire_rx_t *perimeter_p)
{
    /* Wait for ongoing asynchronous convertion to finish. */
    if (!adc_async_wait(&perimeter_p->adc)) {
        return (1);
    }

    return (0);
}

int perimeter_wire_rx_update(struct perimeter_wire_rx_t *perimeter_p)
{
    int i, i_mod;
    /* Input and output buffers holds values in the range -512..511. */
    int input[INPUT_MAX];
    int output[OUTPUT_MAX];
    int min, max;
    float signal, quality;

    /* Save latest sample. */
    memcpy(perimeter_p->updated.samples,
           perimeter_p->ongoing.samples,
           sizeof(perimeter_p->ongoing.samples));

    /* Copy samples to filter input buffer. */
    for (i = 0; i < INPUT_MAX; i++) {
        i_mod = (i % PERIMETER_WIRE_RX_SAMPLES_MAX);
        input[i] = (perimeter_p->updated.samples[i_mod] - 512);
    }

    filter_fir(input,
               membersof(input),
               coefficients,
               membersof(coefficients),
               output);

    /* Get min and max values. */
    min = INT_MAX;
    max = INT_MIN;

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
        quality = -((float)max / min);
    } else {
        signal = min;
        quality = -((float)min / max);
    }

    signal /= NUMBER_OF_NON_ZERO_COEFFICIENTS;

    /* Low pass filtering of the signal. */
    signal = filter_weighted_average(perimeter_p->filter_weight,
                                     perimeter_p->updated.signal,
                                     signal);
    quality = filter_weighted_average(perimeter_p->filter_weight,
                                      perimeter_p->updated.quality,
                                      quality);

    perimeter_p->updated.signal = signal;
    perimeter_p->updated.quality = quality;

    return (0);
}

float perimeter_wire_rx_get_signal(struct perimeter_wire_rx_t *perimeter_p)
{
    return (perimeter_p->updated.signal);
}

float perimeter_wire_rx_get_quality(struct perimeter_wire_rx_t *perimeter_p)
{
    return (perimeter_p->updated.quality);
}

int perimeter_wire_rx_set_filter_weight(struct perimeter_wire_rx_t *perimeter_p,
                                        float weight)
{
    perimeter_p->filter_weight = weight;

    return (0);
}
