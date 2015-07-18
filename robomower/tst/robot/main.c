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

struct motors_data_t {
    float left_wheel_omega;
    float right_wheel_omega;
};

static char robot_stack[300];

/* Testdata fed into the robot from the perimeter_wire_rx stub
   module.*/
int perimeter_testdata_index;
int perimeter_testdata_max;
const float *perimeter_testdata_signal_level;

FAR static const float test_automatic_testdata_signal_level[] = {
    1.0f,
    1.0f,
    1.0f,
    -1.0f, /* Outside perimeter wire. */
    0.0f,
    0.5f,
};

/* Reference data that the motor stub module receives. */
FAR static const struct motors_data_t test_automatic_testdata_direction[] = {
    { .left_wheel_omega = 1.570795f, .right_wheel_omega = 1.570795f },
    { .left_wheel_omega = 1.570795f, .right_wheel_omega = 1.570795f },
    { .left_wheel_omega = 1.570795f, .right_wheel_omega = 1.570795f },
    { .left_wheel_omega = 0.0f, .right_wheel_omega = 0.0f },
    { .left_wheel_omega = -1.570795f, .right_wheel_omega = -1.570795f },
};

struct queue_t motor_queue;

static int test_automatic(struct harness_t *harness_p)
{
    int i;
    float omega;

    perimeter_testdata_index = 0;
    perimeter_testdata_max = membersof(test_automatic_testdata_signal_level);
    perimeter_testdata_signal_level = test_automatic_testdata_signal_level;

    BTASSERT(queue_init(&motor_queue, NULL, 0) == 0);

    robot_init();

    thrd_spawn(robot_entry,
               NULL,
               0,
               robot_stack,
               sizeof(robot_stack));

    robot_manual_start();

    /* Read direction and omega that the robot module passes to the
       motor stub module. */
    for (i = 0; i < membersof(test_automatic_testdata_direction); i++) {
        std_printk(STD_LOG_NOTICE, FSTR("testdata index %d"), i);

        BTASSERT(chan_read(&motor_queue, &omega, sizeof(omega)) == sizeof(omega));
        std_printk(STD_LOG_NOTICE, FSTR("omega = %d"), (int)omega);
        BTASSERT(omega == test_automatic_testdata_direction[i].left_wheel_omega);

        BTASSERT(chan_read(&motor_queue, &omega, sizeof(omega)) == sizeof(omega));
        std_printk(STD_LOG_NOTICE, FSTR("omega = %d"), (int)omega);
        BTASSERT(omega == test_automatic_testdata_direction[i].right_wheel_omega);
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
