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

/* Testdata fed into the robot from the power stub module.*/
int power_testdata_index;
int power_testdata_max;
const int8_t FAR *power_testdata_stored_energy_level;

/* Input signal to robot. */
FAR static const int8_t test_automatic_testdata_stored_energy_level[] = {
    /* 0 */
    100, 90, 80, 70, 60,

    /* 5 */
    50, 40, 30, 30, 30,

    /* 10 */
    20, 20, 20, 20, 40,

    /* 15 */
    60, 80, 90, 100, 100,
};

/* Testdata fed into the robot from the perimeter_wire_rx stub
   module.*/
int perimeter_testdata_index;
int perimeter_testdata_max;
const float FAR *perimeter_testdata_signal_level;

/* Input signal to robot. */
FAR static const float test_automatic_testdata_signal_level[] = {
    /* 0 */
    1.0f,
    1.0f,
    1.0f,
    -1.0f, /* Outside perimeter wire. */
    0.0f,

    /* 5 */
    0.5f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,

    /* 10 */
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,

    /* 15 */
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,

    /* 20 */
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
};

/* Reference data that the motor stub module receives. */
FAR static const struct motors_data_t test_automatic_testdata_direction[] = {
    /* 0 */
    /* cutting forward */
    { .left_wheel_omega =  1.570795f, .right_wheel_omega =  1.570795f },
    { .left_wheel_omega =  1.570795f, .right_wheel_omega =  1.570795f },
    { .left_wheel_omega =  1.570795f, .right_wheel_omega =  1.570795f },
    /* perimeter found. stop.*/
    { .left_wheel_omega =  0.0f,      .right_wheel_omega =  0.0f      },
    /* cutting backwards.*/
    { .left_wheel_omega = -1.570795f, .right_wheel_omega = -1.570795f },

    /* 5 */
    /* stop.*/
    { .left_wheel_omega =  0.0f,      .right_wheel_omega =  0.0f      },
    /* cutting rotate.*/
    { .left_wheel_omega =  0.235619f, .right_wheel_omega = -0.235619f },
    { .left_wheel_omega =  0.0f,      .right_wheel_omega =  0.0f      },
    /* cutting forward.*/
    { .left_wheel_omega =  1.570795f, .right_wheel_omega =  1.570795f },
    { .left_wheel_omega =  1.570795f, .right_wheel_omega =  1.570795f },

    /* 10 */
    /* no power available.*/
    { .left_wheel_omega =  0.0f,      .right_wheel_omega =  0.0f      },
    /* find perimeter wire.*/
    { .left_wheel_omega =  0.05f,     .right_wheel_omega =  0.05f     },
    /* perimeter wire found.*/
    { .left_wheel_omega =  0.0f,      .right_wheel_omega =  0.0f      },
    /* charging in base station.*/
    { .left_wheel_omega =  0.0f,      .right_wheel_omega = 0.0f       },
    { .left_wheel_omega =  0.0f,      .right_wheel_omega = 0.0f       },

    /* 15 */
    { .left_wheel_omega =  0.0f,      .right_wheel_omega = 0.0f       },
    { .left_wheel_omega =  0.0f,      .right_wheel_omega = 0.0f       },
    { .left_wheel_omega =  0.0f,      .right_wheel_omega = 0.0f       },
    { .left_wheel_omega =  0.0f,      .right_wheel_omega = 0.0f       },
    { .left_wheel_omega =  0.0f,      .right_wheel_omega = 0.0f       },

    /* 20 */
    { .left_wheel_omega =  0.0f,      .right_wheel_omega = 0.0f       },
    /* leaving base station. cutting backwards.*/
    { .left_wheel_omega = -1.570795f, .right_wheel_omega = -1.570795f },
};

struct queue_t motor_queue;

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

static int test_automatic(struct harness_t *harness_p)
{
    int i;
    float omega;

    power_testdata_index = 0;
    power_testdata_max = membersof(test_automatic_testdata_stored_energy_level);
    power_testdata_stored_energy_level = test_automatic_testdata_stored_energy_level;

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
#ifdef ARCH_LINUX
        printf("omega = %f\n", omega);
#endif
        BTASSERT(float_close_to_zero(omega
                                     - test_automatic_testdata_direction[i].left_wheel_omega));

        BTASSERT(chan_read(&motor_queue, &omega, sizeof(omega)) == sizeof(omega));
        std_printk(STD_LOG_NOTICE, FSTR("omega = %d"), (int)omega);
#ifdef ARCH_LINUX
        printf("omega = %f\n", omega);
#endif
        BTASSERT(float_close_to_zero(omega -
                                     test_automatic_testdata_direction[i].right_wheel_omega));
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
