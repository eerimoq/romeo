/**
 * @file motor_stub.c
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

extern struct queue_t motor_queue;

static int data_index = 0;

int motor_init(struct motor_t *motor_p,
               struct pin_device_t *in1_p,
               struct pin_device_t *in2_p,
               struct pwm_device_t *enable_p,
               struct adc_device_t *current_adc_p,
               struct pin_device_t *current_dev_p)
{
    return (0);
}

int motor_start(struct motor_t *motor_p)
{
    return (0);
}

int motor_set_direction(struct motor_t *motor_p,
                        int direction)
{
    return (0);
}

int motor_set_omega(struct motor_t *motor_p,
                    float omega)
{
    std_printk(STD_LOG_NOTICE, FSTR("motor_stub: omega = %d"), (int)omega);

    chan_write(&motor_queue, &omega, sizeof(omega));

    return (0);
}

float motor_get_current(struct motor_t *motor_p)
{
    const struct testdata_t FAR *data_p = &testdata_p[data_index / 2];

    data_index++;

    if (data_p->energy_level != -1) {
        std_printk(STD_LOG_NOTICE,
                   FSTR("motor_stub: motor_current = %d"),
                   (int)data_p->motor_current);
        return (data_p->motor_current);
    } else {
        return (0.0f);
    }
}
