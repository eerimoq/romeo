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

static int test_expired(struct harness_t *harness_p)
{
    struct watchdog_t watchdog;

    BTASSERT(watchdog_init(&watchdog, 2) == 0);
    BTASSERT(watchdog_start(&watchdog) == 0);
    BTASSERT(watchdog_tick(&watchdog) == 1);
    BTASSERT(watchdog_tick(&watchdog) == 0);
    BTASSERT(watchdog_stop(&watchdog) == 0);

    return (0);
}

static int test_running(struct harness_t *harness_p)
{
    struct watchdog_t watchdog;

    BTASSERT(watchdog_init(&watchdog, 5) == 0);
    BTASSERT(watchdog_start(&watchdog) == 0);
    BTASSERT(watchdog_tick(&watchdog) == 4);
    BTASSERT(watchdog_tick(&watchdog) == 3);
    BTASSERT(watchdog_kick(&watchdog) == 0);
    BTASSERT(watchdog_tick(&watchdog) == 4);
    BTASSERT(watchdog_tick(&watchdog) == 3);
    BTASSERT(watchdog_tick(&watchdog) == 2);
    BTASSERT(watchdog_stop(&watchdog) == 0);

    return (0);
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t harness_testcases[] = {
        { test_expired, "test_expired" },
        { test_running, "test_running" },
        { NULL, NULL }
    };

    sys_start();
    uart_module_init();

    harness_init(&harness);
    harness_run(&harness, harness_testcases);

    return (0);
}
