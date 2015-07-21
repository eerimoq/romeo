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

extern int FS_PARAMETER(robot_parameter_charging);

#include "simba.h"
#include "robomower.h"
#include "testdata.h"

static char robot_stack[300];

struct queue_t motor_queue;

static int float_close_to_zero(float value)
{
    return ((value > -0.001) && (value < 0.001));
}

static int test_automatic(struct harness_t *harness_p)
{
    int i;
    float omega;
    const struct testdata_t FAR *data_p;

    testdata_index = 0;
    testdata_p = test_automatic_testdata;

    BTASSERT(queue_init(&motor_queue, NULL, 0) == 0);

    robot_init();

    thrd_spawn(robot_entry,
               NULL,
               0,
               robot_stack,
               sizeof(robot_stack));

    FS_PARAMETER(robot_parameter_charging) = 1;

    robot_manual_start();

    /* Read direction and omega that the robot module passes to the
       motor stub module. */
    i = 0;
    data_p = &test_automatic_testdata[0];

    while (data_p->energy_level != -1) {
        /* Left wheel. */
        BTASSERT(chan_read(&motor_queue, &omega, sizeof(omega)) == sizeof(omega));
        BTASSERT(float_close_to_zero(omega - data_p->left_wheel_omega), 
                 "[%d]: %f %f",
                 i,
                 omega,
                 data_p->left_wheel_omega);

        /* Right wheel. */
        BTASSERT(chan_read(&motor_queue, &omega, sizeof(omega)) == sizeof(omega));
        BTASSERT(float_close_to_zero(omega - data_p->right_wheel_omega), 
                 "[%d]: %f %f",
                 i,
                 omega,
                 data_p->right_wheel_omega);

        i++;
        data_p++;
    }

    return (0);
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t harness_testcases[] = {
        { test_automatic, "test_automatic" },
        { NULL, NULL }
    };

    sys_start();
    uart_module_init();

    harness_init(&harness);
    harness_run(&harness, harness_testcases);

    return (0);
}
