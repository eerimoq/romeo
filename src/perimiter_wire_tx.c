/**
 * @file robomower/perimiter_wire_tx.c
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

FS_COUNTER(perimiter_timer_callback);

/* The signal transmitted on the wire. */
static uint8_t output_signal[] = {
    1, 1, 0, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 1, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 1, 0
};

static struct timer_t timer;
static struct perimiter_wire_tx_t pwires[PERIMITER_WIRE_TX_MAX];

static void timer_callback(void *arg_p)
{
    int i;
    uint8_t value;
    struct perimiter_wire_tx_t *pwire_p;

    for (i = 0; i < membersof(pwires); i++) {
        pwire_p = &pwires[i];

        if (pwire_p->state != PERIMITER_WIRE_TX_STATE_ON) {
            continue;
        }

        /* Set signal level on wire. */
        value = output_signal[pwire_p->pos];
        pin_write(&pwire_p->pin_in1, !value);
        pin_write(&pwire_p->pin_in2, value);

        /* Update pos for next timeout. */
        pwire_p->pos++;
        if (pwire_p->pos == membersof(output_signal)) {
            pwire_p->pos = 0;
        }
    }

    FS_COUNTER_INC(perimiter_timer_callback, 1);
}

int perimiter_wire_tx_module_init(void)
{
    int i;
    struct time_t timeout;

    for (i = 0; i < membersof(pwires); i++) {
        pwires[i].state = PERIMITER_WIRE_TX_STATE_UNUSED;
    }

    /* Start the perimiter wire sender timer. */
    timeout.seconds = 0;
    timeout.nanoseconds = 1000000L;

    return (timer_set(&timer,
                      &timeout,
                      timer_callback,
                      NULL,
                      TIMER_PERIODIC));
}

int perimiter_wire_tx_init(struct perimiter_wire_tx_t *pwire_p,
                           struct pin_device_t *pin_dev_in1_p,
                           struct pin_device_t *pin_dev_in2_p)
{
    pwire_p->state = PERIMITER_WIRE_TX_STATE_OFF;
    pwire_p->pos = 0;

    /* Setup the pins. */
    pin_init(&pwire_p->pin_in1, pin_dev_in1_p, PIN_OUTPUT);
    pin_init(&pwire_p->pin_in2, pin_dev_in2_p, PIN_OUTPUT);

    return (0);
}

int perimiter_wire_tx_start(struct perimiter_wire_tx_t *pwire_p)
{
    std_printk(STD_LOG_NOTICE,
               FSTR("Starting transmitting signal on perimiter wire"));

    pwire_p->state = PERIMITER_WIRE_TX_STATE_ON;

    return (0);
}
