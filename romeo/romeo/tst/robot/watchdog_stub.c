/**
 * @file watchdog_stub.c
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

int watchdog_init(struct watchdog_t *watchdog_p,
                  int timeout)
{
    return (0);
}

int watchdog_start(struct watchdog_t *watchdog_p)
{
    return (0);
}

int watchdog_stop(struct watchdog_t *watchdog_p)
{
    return (0);
}

int watchdog_tick(struct watchdog_t *watchdog_p)
{
    std_printf(FSTR("watchdog_stub: tick\r\n"));

    return (1);
}

int watchdog_kick(struct watchdog_t *watchdog_p)
{
    std_printf(FSTR("watchdog_stub: kicked\r\n"));

    return (0);
}
