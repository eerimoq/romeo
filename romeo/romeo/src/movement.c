/**
 * @file romeo/movement.c
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

static struct fs_counter_t counter_movement_left_omega_too_big;
static struct fs_counter_t counter_movement_right_omega_too_big;

/**
 * Robot sketch with definitions of velocity and angular velocity
 * directions.
 *
 * X = wheels
 * v = velocity forwards
 * w = angular velocity, positive for clock wise rotation.
 *
 * lw = left wheel
 * rw = right wheel
 *
 *
 *             +----------------------+
 *             |                      |
 *            X|          ^ v         |X
 *        lw  X|          |           |X  rw
 *            X|      ^   |   \       |X
 *             |    w |   O   | w     |
 *             |      \       v       |
 *             |                      |
 *             |          X           |
 *             |          X           |
 *             +----------------------+
 */

/**
 * Limit wheel omega to the maximum motor omega.
 * @param[in,out] omega wheel omega
 * @return true(1) if omega already was within the allowed range and
 *         false(0) if it was outside the range.
 */
static int limit_wheel_omega_to_motor_omega(float *omega_p)
{
    if (*omega_p > MOTOR_OMEGA_MAX) {
        *omega_p = MOTOR_OMEGA_MAX;
        return (0);
    } else if (*omega_p < -MOTOR_OMEGA_MAX) {
        *omega_p = -MOTOR_OMEGA_MAX;
        return (0);
    }

    return (1);
}

/**
 * Calculate wheel omega (rad/s) from wheel speed (m/s).
 */
static float speed_to_omega(struct movement_t *movement_p,
                            float speed)
{
    return (speed / movement_p->wheel_radius_in_meters);
}

int movement_module_init()
{
    fs_counter_init(&counter_movement_left_omega_too_big,
                    FSTR("/robot/counters/movement_left_omega_too_big"),
                    0);
    fs_counter_register(&counter_movement_left_omega_too_big);

    fs_counter_init(&counter_movement_right_omega_too_big,
                    FSTR("/robot/counters/movement_right_omega_too_big"),
                    0);
    fs_counter_register(&counter_movement_right_omega_too_big);

    return (0);
}

int movement_init(struct movement_t *movement_p,
                  float wheel_distance_in_meters,
                  float wheel_radius_in_meters)
{
    movement_p->wheel_distance_in_meters = wheel_distance_in_meters;
    movement_p->wheel_radius_in_meters = wheel_radius_in_meters;

    return (0);
}

int movement_calculate_wheels_omega(struct movement_t *movement_p,
                                    float speed,
                                    float omega,
                                    float *left_wheel_omega_p,
                                    float *right_wheel_omega_p)
{
    float left_wheel_speed;
    float right_wheel_speed;
    float left_wheel_omega;
    float right_wheel_omega;
    float omega_wheel_speed;

    *left_wheel_omega_p = 0.0f;
    *right_wheel_omega_p = 0.0f;

    /* The base speed of the wheels. */
    left_wheel_speed = speed;
    right_wheel_speed = speed;

    /* Omega gives the difference in speed for the two motors. */
    omega_wheel_speed =
        (omega * movement_p->wheel_distance_in_meters / 2.0f);

    /* Add the speed component from omega. */
    left_wheel_speed += omega_wheel_speed;
    right_wheel_speed -= omega_wheel_speed;

    /* Calculate output values in radians/s. */
    left_wheel_omega = speed_to_omega(movement_p, left_wheel_speed);
    right_wheel_omega = speed_to_omega(movement_p, right_wheel_speed);

    /* Omega cannot be bigger than the maximum motor omega. */
    if (limit_wheel_omega_to_motor_omega(&left_wheel_omega) != 1) {
        fs_counter_increment(&counter_movement_left_omega_too_big, 1);
    }

    if (limit_wheel_omega_to_motor_omega(&right_wheel_omega) != 1) {
        fs_counter_increment(&counter_movement_right_omega_too_big, 1);
    }

    *left_wheel_omega_p = left_wheel_omega;
    *right_wheel_omega_p = right_wheel_omega;

    return (0);
}

float movement_get_maximum_speed(struct movement_t *movement_p)
{
    return (MOTOR_OMEGA_MAX * movement_p->wheel_radius_in_meters);
}

float movement_get_maximum_omega(struct movement_t *movement_p)
{
    return (movement_get_maximum_speed(movement_p)
            / (movement_p->wheel_distance_in_meters / 2.0f));
}
