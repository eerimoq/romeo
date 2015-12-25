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
    float reference;
    float actual;
    float control;
};

static struct testdata_t testdata_1_0_0[] = {
    { .reference = 5.0f, .actual = 0.0f, .control =  5.0f },
    { .reference = 5.0f, .actual = 2.0f, .control =  3.0f },
    { .reference = 5.0f, .actual = 4.0f, .control =  1.0f },
    { .reference = 5.0f, .actual = 6.0f, .control = -1.0f },
    { .reference = 5.0f, .actual = 7.0f, .control = -2.0f },
    { .reference = 5.0f, .actual = 6.0f, .control = -1.0f },
    { .reference = 5.0f, .actual = 5.0f, .control =  0.0f },
    { .reference = 5.0f, .actual = 4.5f, .control =  0.5f },
    { .reference = 5.0f, .actual = 4.5f, .control =  0.5f },
    { .reference = 5.0f, .actual = 4.8f, .control =  0.2f },
    { .reference = 5.0f, .actual = 5.0f, .control =  0.0f },
};

static struct testdata_t testdata_2_05_05[] = {
    { .reference = 5.0f, .actual = 0.0f, .control =  15.0f   },
    { .reference = 5.0f, .actual = 4.0f, .control =   3.0f   },
    { .reference = 5.0f, .actual = 6.0f, .control =  -0.5f   },
    { .reference = 5.0f, .actual = 6.0f, .control =   0.0f   },
    { .reference = 5.0f, .actual = 5.5f, .control =   1.0f   },
    { .reference = 5.0f, .actual = 5.8f, .control =  -0.4f   },
    { .reference = 5.0f, .actual = 5.5f, .control =   0.25f  },
    { .reference = 5.0f, .actual = 5.6f, .control =  -0.45f  },
    { .reference = 5.0f, .actual = 5.3f, .control =   0.199f },
};

static int float_close_to_zero(float value)
{
    return ((value > -0.001) && (value < 0.001));
}

static int test_pid_x_y_x(struct testdata_t *data_p,
                          size_t data_length,
                          float kp,
                          float ki,
                          float kd)
{
    int i;
    float control;
    struct controller_pid_t pid;

    BTASSERT(controller_pid_init(&pid, kp, ki, kd) == 0);

    for (i = 0; i < data_length; i++, data_p++) {
        control = controller_pid_calculate(&pid, data_p->reference, data_p->actual);
        BTASSERT(float_close_to_zero(control - data_p->control),
                 "[%d]: %f %f", i, control, data_p->control);
    }

    return (0);
}

static int test_pid_1_0_0(struct harness_t *harness_p)
{
    return (test_pid_x_y_x(testdata_1_0_0,
                           membersof(testdata_1_0_0),
                           1.0f,
                           0.0f,
                           0.0f));
}

static int test_pid_2_05_05(struct harness_t *harness_p)
{
    return (test_pid_x_y_x(testdata_2_05_05,
                           membersof(testdata_2_05_05),
                           2.0f,
                           0.5f,
                           0.5f));
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t harness_testcases[] = {
        { test_pid_1_0_0, "test_pid_1_0_0" },
        { test_pid_2_05_05, "test_pid_2_05_05" },
        { NULL, NULL }
    };

    sys_start();
    uart_module_init();

    harness_init(&harness);
    harness_run(&harness, harness_testcases);

    return (0);
}
