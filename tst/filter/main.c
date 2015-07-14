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

#define NUMBER_OF_INPUT_SAMPLES 80
#define NUMBER_OF_COEFFICIENTS 24
#define NUMBER_OF_OUTPUT_SAMPLES \
    (NUMBER_OF_INPUT_SAMPLES - NUMBER_OF_COEFFICIENTS + 1)

/* Float test data. */
static const float f_input[NUMBER_OF_INPUT_SAMPLES] = {
    /* sample 0-23 */
    1.0, 0.0, -1.0, 0.0, 1.0, -1.0, 1.0, -1.0,
    0.0, 1.0, -1.0, 1.0, 0.0, -1.0, 0.0, 1.0,
    -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 0.0, -1.0,

    /* sample 24-47 */
    1.0, 0.0, -1.0, 0.0, 1.0, -1.0, 1.0, -1.0,
    0.0, 1.0, -1.0, 1.0, 0.0, -1.0, 0.0, 1.0,
    -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 0.0, -1.0,

    /* sample 48-71 */
    1.0, 0.0, -1.0, 0.0, 1.0, -1.0, 1.0, -1.0,
    0.0, 1.0, -1.0, 1.0, 0.0, -1.0, 0.0, 1.0,
    -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 0.0, -1.0,

    /* sample 72-79 */
    1.0, 0.0, -1.0, 0.0, 1.0, -1.0, 1.0, -1.0,
};

static const float f_coefficients[NUMBER_OF_COEFFICIENTS] = {
    1.0, 1.0, -1.0, -1.0, 1.0, -1.0, 1.0, -1.0,
    -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0,
    -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0,
};

/* Integer test data. */
static const int i_input[NUMBER_OF_INPUT_SAMPLES] = {
    /* sample 0-23 */
    1, 0, -1, 0, 1, -1, 1, -1,
    0, 1, -1, 1, 0, -1, 0, 1,
    -1, 0, 1, -1, 0, 1, 0, -1,

    /* sample 24-47 */
    1, 0, -1, 0, 1, -1, 1, -1,
    0, 1, -1, 1, 0, -1, 0, 1,
    -1, 0, 1, -1, 0, 1, 0, -1,

    /* sample 48-71 */
    1, 0, -1, 0, 1, -1, 1, -1,
    0, 1, -1, 1, 0, -1, 0, 1,
    -1, 0, 1, -1, 0, 1, 0, -1,

    /* sample 72-79 */
    1, 0, -1, 0, 1, -1, 1, -1,
};

static const int i_coefficients[NUMBER_OF_COEFFICIENTS] = {
    1, 1, -1, -1, 1, -1, 1, -1,
    -1, 1, -1, 1, 1, -1, -1, 1,
    -1, -1, 1, -1, -1, 1, 1, -1,
};

static int test_firf(struct harness_t *harness_p)
{
    int i;
    float input_sign_changed[NUMBER_OF_INPUT_SAMPLES];
    float output[NUMBER_OF_OUTPUT_SAMPLES];
    float output_sign_changed[NUMBER_OF_OUTPUT_SAMPLES];

    BTASSERT(filter_firf(f_input,
                         NUMBER_OF_INPUT_SAMPLES,
                         f_coefficients,
                         NUMBER_OF_COEFFICIENTS,
                         output) == 0);

    /* Input data and filter coefficients match perfectly every 24th
       sample.*/
    BTASSERT((int)output[0] == 16);
    BTASSERT((int)output[24] == 16);
    BTASSERT((int)output[48] == 16);

    /* Change sign on input data and verify that the output also
       changes sign. */
    for (i = 0; i < NUMBER_OF_INPUT_SAMPLES; i++) {
        input_sign_changed[i] = -f_input[i];
    }

    BTASSERT(filter_firf(input_sign_changed,
                         NUMBER_OF_INPUT_SAMPLES,
                         f_coefficients,
                         NUMBER_OF_COEFFICIENTS,
                         output_sign_changed) == 0);

    for (i = 0; i < NUMBER_OF_OUTPUT_SAMPLES; i++) {
        BTASSERT(output[i] == -output_sign_changed[i], "i = %d", i);
    }

    return (0);
}

static int test_fir(struct harness_t *harness_p)
{
    int i;
    int input_sign_changed[NUMBER_OF_INPUT_SAMPLES];
    int output[NUMBER_OF_OUTPUT_SAMPLES];
    int output_sign_changed[NUMBER_OF_OUTPUT_SAMPLES];

    BTASSERT(filter_fir(i_input,
                        NUMBER_OF_INPUT_SAMPLES,
                        i_coefficients,
                        NUMBER_OF_COEFFICIENTS,
                        output) == 0);

    /* Input data and filter coefficients match perfectly every 24th
       sample.*/
    BTASSERT(output[0] == 16);
    BTASSERT(output[24] == 16);
    BTASSERT(output[48] == 16);

    /* Change sign on input data and verify that the output also
       changes sign. */
    for (i = 0; i < NUMBER_OF_INPUT_SAMPLES; i++) {
        input_sign_changed[i] = -i_input[i];
    }

    BTASSERT(filter_fir(input_sign_changed,
                        NUMBER_OF_INPUT_SAMPLES,
                        i_coefficients,
                        NUMBER_OF_COEFFICIENTS,
                        output_sign_changed) == 0);

    for (i = 0; i < NUMBER_OF_OUTPUT_SAMPLES; i++) {
        BTASSERT(output[i] == -output_sign_changed[i], "i = %d", i);
    }

    return (0);
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t harness_testcases[] = {
        { test_firf, "test_firf" },
        { test_fir, "test_fir" },
        { NULL, NULL }
    };

    sys_start();
    uart_module_init();

    harness_init(&harness);
    harness_run(&harness, harness_testcases);

    return (0);
}
