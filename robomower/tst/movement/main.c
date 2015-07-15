/**
 * @file main.c
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

static int float_close_to_zero(float value)
{
    return ((value > -0.001) && (value < 0.001));
}

static int test_calculate_wheels_omega(struct harness_t *harness_p)
{
    float left_wheel_omega;
    float right_wheel_omega;

    /* Standing still. */
    movement_calculate_wheels_omega(0.0f,
                                    0.0f,
                                    &left_wheel_omega,
                                    &right_wheel_omega);
    BTASSERT(left_wheel_omega == 0.0f);
    BTASSERT(right_wheel_omega == 0.0f);

    /* Moving in a straight line with a speed of 40 cm/s. */
    movement_calculate_wheels_omega(40.0f,
                                    0.0f,
                                    &left_wheel_omega,
                                    &right_wheel_omega);
    BTASSERT(float_close_to_zero(left_wheel_omega - 628.318f));
    BTASSERT(float_close_to_zero(right_wheel_omega - 628.318f));

    /* Moving in a straight line with a speed of -40 cm/s. */
    movement_calculate_wheels_omega(-40.0f,
                                    0.0f,
                                    &left_wheel_omega,
                                    &right_wheel_omega);
    BTASSERT(float_close_to_zero(left_wheel_omega + 628.318f));
    BTASSERT(float_close_to_zero(right_wheel_omega + 628.318f));

    /* Truning in place. A complete revolution in one second. */
    movement_calculate_wheels_omega(0.0f,
                                    628.318f,
                                    &left_wheel_omega,
                                    &right_wheel_omega);
    BTASSERT(float_close_to_zero(left_wheel_omega - 1480.438f));
    BTASSERT(float_close_to_zero(right_wheel_omega + 1480.438f));

    /* Combined turning and speed. */
    movement_calculate_wheels_omega(40.0f,
                                    628.318f,
                                    &left_wheel_omega,
                                    &right_wheel_omega);
    BTASSERT(float_close_to_zero(left_wheel_omega - 1480.438f - 628.318f));
    BTASSERT(float_close_to_zero(right_wheel_omega + 1480.438f - 628.318f));

    return (0);
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t harness_testcases[] = {
        { test_calculate_wheels_omega, "test_calculate_wheels_omega" },
        { NULL, NULL }
    };

    sys_start();
    uart_module_init();

    harness_init(&harness);
    harness_run(&harness, harness_testcases);

    return (0);
}
