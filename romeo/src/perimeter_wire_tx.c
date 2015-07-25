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

FS_COUNTER_DEFINE(perimeter_timer_callback);

#define PERIOD_NS 1000000L

/* The signal transmitted on the wire. */
static uint8_t output_signal[] = {
    1, 1, 0, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 1, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 1, 0
};

static struct timer_t timer;
static struct perimeter_wire_tx_t *perimeter_wires[PERIMITER_WIRE_TX_MAX];

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

    FS_COUNTER_INC(perimeter_timer_callback, 1);
}

int perimeter_wire_tx_module_init(void)
{
    int i;
    struct time_t timeout;

    for (i = 0; i < membersof(perimeter_wires); i++) {
        perimeter_wires[i] = NULL;
    }

    /* Start the perimeter wire sender timer. */
    timeout.seconds = 0;
    timeout.nanoseconds = PERIOD_NS;

    return (timer_set(&timer,
                      &timeout,
                      timer_callback,
                      NULL,
                      TIMER_PERIODIC));
}

int perimeter_wire_tx_init(struct perimeter_wire_tx_t *perimeter_wire_p,
                           struct pin_device_t *pin_dev_in1_p,
                           struct pin_device_t *pin_dev_in2_p)
{
    perimeter_wire_p->pos = 0;

    /* Setup the pins. */
    pin_init(&perimeter_wire_p->pin_in1, pin_dev_in1_p, PIN_OUTPUT);
    pin_init(&perimeter_wire_p->pin_in2, pin_dev_in2_p, PIN_OUTPUT);

    return (0);
}

int perimeter_wire_tx_start(struct perimeter_wire_tx_t *perimeter_wire_p)
{
    std_printk(STD_LOG_NOTICE,
               FSTR("Starting transmitting signal on perimeter wire"));

    perimeter_wires[0] = perimeter_wire_p;

    return (0);
}
