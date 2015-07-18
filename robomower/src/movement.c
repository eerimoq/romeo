/**
 * @file robomower/movement.c
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

/* Robot geometry. */
#define WHEEL_DISTANCE_M      0.3f
#define WHEEL_CIRCUMFERENCE_M 0.4f

/* Maximum wheel rotation speed in rad/s. */
#define WHEEL_OMEGA_MAX       PI

/**
 * Check if omega is within allowed range.
 * @return true(1) in omega is within allowed range, otherwise false(0)
 */
static int is_omega_in_range(float omega)
{
    if ((omega > WHEEL_OMEGA_MAX) || (omega < -WHEEL_OMEGA_MAX)) {
        std_printk(STD_LOG_NOTICE, FSTR("wheel omega outside allowed range"));
        return (0);
    }

    return (1);
}

int movement_calculate_wheels_omega(float speed,
                                    float omega,
                                    float *left_wheel_omega,
                                    float *right_wheel_omega)
{
    float left_wheel_speed;
    float right_wheel_speed;
    float omega_wheel_speed;
    int res = 0;

    /* The base speed of the wheels. */
    left_wheel_speed = speed;
    right_wheel_speed = speed;

    /* Omega gives the difference in speed for the two motors. */
    omega_wheel_speed = (omega * WHEEL_DISTANCE_M / 2.0f);

    /* Add/subtract the speed component from omega. */
    if (omega > 0) {
        left_wheel_speed += omega_wheel_speed;
        right_wheel_speed -= omega_wheel_speed;
    } else {
        left_wheel_speed -= omega_wheel_speed;
        right_wheel_speed += omega_wheel_speed;
    }

    /* Calculate output values in radians/s. */
    *left_wheel_omega = (2.0f * PI * left_wheel_speed / WHEEL_CIRCUMFERENCE_M);
    *right_wheel_omega = (2.0f * PI * right_wheel_speed / WHEEL_CIRCUMFERENCE_M);

    /* range checks. */
    if (!is_omega_in_range(*left_wheel_omega)) {
        res = -1;
    }

    if (!is_omega_in_range(*right_wheel_omega)) {
        res = -1;
    }

    return (res);
}
