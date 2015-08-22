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

extern int FS_PARAMETER(robot_parameter_charging);

#include "simba.h"
#include "romeo.h"
#include "testdata.h"

extern int motor_stub_omega_next;
extern float motor_stub_omega[16];

extern int motor_stub_current_next;
extern float motor_stub_current[16];

extern int perimeter_wire_rx_stub_signal_next;
extern float perimeter_wire_rx_stub_signal[16];

extern int battery_stub_energy_level_next;
extern int battery_stub_energy_level[16];

static int float_close_to_zero(float value)
{
    return ((value > -0.001) && (value < 0.001));
}

static int test_automatic(struct harness_t *harness_p)
{
    int i;
    float omega;
    struct robot_t robot;
    const struct testdata_t FAR *testdata_p;

    FS_PARAMETER(robot_parameter_charging) = 1;

    robot_init(&robot);
    robot_start(&robot);

    testdata_p = test_automatic_testdata;

    /* Tick the robot to enter cutting state. */
    robot_tick(&robot);
    robot_tick(&robot);
    robot_tick(&robot);

    /* The actual verification of robot behaviour is done in the
       stubs. */
    i = 0;

    while (testdata_p->input.energy_level != -1) {
        std_printk(STD_LOG_NOTICE, FSTR("id: %d"), testdata_p->id);

        /* Prepare the stubs. */
        motor_stub_omega_next = 0;
        motor_stub_current_next = 0;
        perimeter_wire_rx_stub_signal_next = 0;
        battery_stub_energy_level_next = 0;

        motor_stub_current[0] = testdata_p->input.motor_current;
        perimeter_wire_rx_stub_signal[0] = testdata_p->input.perimeter_signal;
        battery_stub_energy_level[0] = testdata_p->input.energy_level;

        /* Tick the robot. */
        robot_tick(&robot);

        if (testdata_p->output.compare == 1) {
            /* Compare output to reference data. */
            BTASSERT(motor_stub_omega_next == 2, "next = %d", motor_stub_omega_next);
            omega = motor_stub_omega[0];
            BTASSERT(float_close_to_zero(omega - testdata_p->output.left_wheel_omega), 
                     "[%d]: from robot: %f, testdata: %f",
                     i,
                     omega,
                     testdata_p->output.left_wheel_omega);
            omega = motor_stub_omega[1];
            BTASSERT(float_close_to_zero(omega - testdata_p->output.right_wheel_omega), 
                     "[%d]: %f %f",
                     i,
                     omega,
                     testdata_p->output.right_wheel_omega);
        }

        i++;
        testdata_p++;
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
