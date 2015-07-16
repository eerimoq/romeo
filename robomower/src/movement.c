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
#define WHEEL_DISTANCE_CM      30.0f
#define WHEEL_CIRCUMFERENCE_CM 40.0f

int movement_calculate_wheels_omega(float speed,
                                    float omega,
                                    float *left_wheel_omega,
                                    float *right_wheel_omega)
{
    float left_wheel_speed;
    float right_wheel_speed;
    float omega_wheel_speed;

    /* The base speed of the wheels. */
    left_wheel_speed = speed;
    right_wheel_speed = speed;

    /* Omega gives the difference in speed for the two motors. */
    omega_wheel_speed = (omega * WHEEL_DISTANCE_CM / 2.0f);

    /* Add/subtract the speed component from omega. */
    if (omega > 0) {
        left_wheel_speed += omega_wheel_speed;
        right_wheel_speed -= omega_wheel_speed;
    } else {
        left_wheel_speed -= omega_wheel_speed;
        right_wheel_speed += omega_wheel_speed;
    }

    /* Calculate output values in radians/s. */
    *left_wheel_omega = (2.0f * PI * left_wheel_speed / WHEEL_CIRCUMFERENCE_CM);
    *right_wheel_omega = (2.0f * PI * right_wheel_speed / WHEEL_CIRCUMFERENCE_CM);

    return (0);
}
