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
 * This file is part of the Romeo project.
 */

#include "simba.h"
#include "romeo.h"

struct testdata_t {
    float speed;
    float omega;
    float left_wheel_omega;
    float right_wheel_omega;
};

static int float_close_to_zero(float value)
{
    if (!((value > -0.001) && (value < 0.001))) {
#ifdef ARCH_LINUX
        /* No float support in std module. */
        printf("%f\n", value);
#endif
        return (0);
    }

    return (1);
}

static int test_calculate_wheels_omega(struct harness_t *harness_p)
{
    float left_wheel_omega;
    float right_wheel_omega;
    int i;

    static struct testdata_t testdata[] = {
        /* Standing still. */
        { 0.0f, 0.0f, 0.0f, 0.0f },
        /* Moving in a straight line with a speed of 0.2 m/s. */
        { 0.2f, 0.0f, PI, PI },
        /* Moving in a straight line with a speed of -0.2 m/s. */
        { -0.2f, 0.0f, -PI, -PI },
        /* Truning in place with the angular velocity 1.0 rad/s. */
        { 0.0f, 1.0f, 2.356f, -2.356f },
        /* Combined turning and speed. */
        { 0.1f, 0.1f, 1.8064f, 1.3352f }
    };

    for (i = 0; i < membersof(testdata); i++) {
        std_printf(FSTR("testdata index: %d\r\n"), i);
        movement_calculate_wheels_omega(testdata[i].speed,
                                        testdata[i].omega,
                                        &left_wheel_omega,
                                        &right_wheel_omega);
        BTASSERT(float_close_to_zero(left_wheel_omega
                                     - testdata[i].left_wheel_omega));
        BTASSERT(float_close_to_zero(right_wheel_omega
                                     - testdata[i].right_wheel_omega));
    }

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
