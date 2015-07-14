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
#include "filter_test.i"

#define DESCRIPTION_MAX 64

static int float_close_to_zero(float value)
{
    return ((value > -0.001) && (value < 0.001));
}

static void copy_test_to_ram(struct filter_test_t *test,
                             char *description,
                             float *input,
                             float *coefficients,
                             float *ref_output)
{
    int i;
    
    for (i = 0; i < DESCRIPTION_MAX; i++) {
        description[i] = test->description[i];
    }

    for (i = 0; i < NUMBER_OF_INPUT_SAMPLES; i++) {
        input[i] = test->input[i];
    }

    for (i = 0; i < NUMBER_OF_COEFFICIENTS; i++) {
        coefficients[i] = test->coefficients[i];
    }

    for (i = 0; i < NUMBER_OF_OUTPUT_SAMPLES; i++) {
        ref_output[i] = test->ref_output[i];
    }
}

static int test_firf(struct harness_t *harness_p)
{
    int i;
    char description[64];
    float input[NUMBER_OF_INPUT_SAMPLES];
    float coefficients[NUMBER_OF_COEFFICIENTS];
    float ref_output[NUMBER_OF_OUTPUT_SAMPLES];
    float output[NUMBER_OF_OUTPUT_SAMPLES];
    struct filter_test_t *test;

    for (test = &filter_test[0]; test->description != NULL; test++) {
        copy_test_to_ram(test,
                         description,
                         input,
                         coefficients,
                         ref_output);
        std_printf(FSTR("%s\n"), description);

        BTASSERT(filter_firf(test->input,
                             NUMBER_OF_INPUT_SAMPLES,
                             test->coefficients,
                             NUMBER_OF_COEFFICIENTS,
                             output) == 0);

        for (i = 0; i < membersof(output); i++) {
            BTASSERT(float_close_to_zero(test->ref_output[i] - output[i]),
                     "i = %d", i);
        }
    }

    return (0);
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t harness_testcases[] = {
        { test_firf, "test_firf" },
        { NULL, NULL }
    };

    sys_start();
    uart_module_init();

    harness_init(&harness);
    harness_run(&harness, harness_testcases);

    return (0);
}
