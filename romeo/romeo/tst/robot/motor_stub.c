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
 * This file is part of the Romeo project.
 */

#include "simba.h"
#include "romeo.h"

int motor_stub_omega_next;
float motor_stub_omega[16];

int motor_stub_current_next;
float motor_stub_current[16];

int motor_init(struct motor_t *motor_p,
               struct pin_device_t *in1_p,
               struct pin_device_t *in2_p,
               struct pwm_device_t *enable_p,
               struct adc_device_t *current_adc_p,
               struct pin_device_t *current_dev_p)
{
    return (0);
}

int motor_async_convert(struct motor_t *motor_p)
{
    return (0);
}

int motor_async_wait(struct motor_t *motor_p)
{
    return (0);
}

int motor_update(struct motor_t *motor_p)
{
    return (0);
}

float motor_get_current(struct motor_t *motor_p)
{
    float current;

    current = motor_stub_current[0];

    std_printf(FSTR("motor_stub: motor_current = %d\r\n"), (int)current);

    return (current);
}

int motor_set_filter_weight(struct motor_t *motor_p,
                            float weight)
{
    motor_p->filter_weight = weight;

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
    std_printf(FSTR("motor_stub: robot called motor_set_omega(%f)\r\n"), omega);

    motor_stub_omega[motor_stub_omega_next++] = omega;

    return (0);
}
