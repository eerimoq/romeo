/**
 * @file romeo/perimeter_wire_tx.c
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

static struct fs_counter_t counter_perimeter_timer_callback;

#define PERIOD_NS 100000L

/* The signal transmitted on the wire. */
static uint8_t output_signal[] = {
    1, 1, 0, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 1, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 1, 0
};

static struct timer_t timer;
static struct perimeter_wire_tx_t *perimeter_wires[PERIMITER_WIRE_TX_MAX];

/**
 * Timer callback called in interrupt context.
 */
static void timer_callback(void *arg_p)
{
    int i;
    uint8_t value;
    struct perimeter_wire_tx_t *perimeter_wire_p;

    for (i = 0; i < membersof(perimeter_wires); i++) {
        perimeter_wire_p = perimeter_wires[i];

        if (perimeter_wire_p == NULL) {
            continue;
        }

        /* Set signal level on wire. */
        value = output_signal[perimeter_wire_p->pos];
        pin_write(&perimeter_wire_p->pin_in1, !value);
        pin_write(&perimeter_wire_p->pin_in2, value);

        /* Update pos for next timeout. */
        perimeter_wire_p->pos++;
        if (perimeter_wire_p->pos == membersof(output_signal)) {
            perimeter_wire_p->pos = 0;
        }
    }

    fs_counter_increment(&counter_perimeter_timer_callback, 1);
}

int perimeter_wire_tx_module_init(void)
{
    int i;
    struct time_t timeout;

    fs_counter_init(&counter_perimeter_timer_callback,
                    FSTR("/robot/counters/perimeter_timer_callback"),
                    0);
    fs_counter_register(&counter_perimeter_timer_callback);

    for (i = 0; i < membersof(perimeter_wires); i++) {
        perimeter_wires[i] = NULL;
    }

    /* Start the perimeter wire sender timer. */
    timeout.seconds = 0;
    timeout.nanoseconds = PERIOD_NS;

    timer_init(&timer,
               &timeout,
               timer_callback,
               NULL,
               TIMER_PERIODIC);

    return (timer_start(&timer));
}

int perimeter_wire_tx_init(struct perimeter_wire_tx_t *perimeter_wire_p,
                           struct pin_device_t *pin_dev_in1_p,
                           struct pin_device_t *pin_dev_in2_p,
                           struct adc_device_t *current_adc_p,
                           struct pin_device_t *current_dev_p)
{
    perimeter_wire_p->pos = 0;

    /* Setup the pins. */
    pin_init(&perimeter_wire_p->pin_in1, pin_dev_in1_p, PIN_OUTPUT);
    pin_init(&perimeter_wire_p->pin_in2, pin_dev_in2_p, PIN_OUTPUT);

    adc_init(&perimeter_wire_p->current_adc,
             current_adc_p,
             current_dev_p,
             ADC_REFERENCE_VCC,
             1000);

    return (0);
}

int perimeter_wire_tx_start(struct perimeter_wire_tx_t *perimeter_wire_p)
{
    std_printf(FSTR("Starting transmitting signal on perimeter wire\r\n"));

    perimeter_wires[0] = perimeter_wire_p;

    return (0);
}

float perimeter_wire_tx_get_current(struct perimeter_wire_tx_t *perimeter_wire_p)
{
    uint16_t sample;

    adc_convert(&perimeter_wire_p->current_adc, &sample, 1);
    perimeter_wire_p->current = ((sample * 30.0f) / 1024.0f);

    //std_printf(FSTR("sample = %d\r\n"), sample);

    return (perimeter_wire_p->current);
}
