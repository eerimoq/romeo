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

static struct uart_driver_t uart;
static char qinbuf[32];
static struct shell_args_t shell_args;

static char shell_stack[350];

static int init()
{
    sys_start();
    uart_module_init();

    /* Setup UART. */
    uart_init(&uart, &uart_device[0], 38400, qinbuf, sizeof(qinbuf));
    uart_start(&uart);
    sys_set_stdout(&uart.chout);

    /* Start the shell. */
    shell_args.chin_p = &uart.chin;
    shell_args.chout_p = &uart.chout;
    thrd_spawn(shell_entry,
               &shell_args,
               20,
               shell_stack,
               sizeof(shell_stack));

    return (0);
}

int main()
{
    init();

    return (robot_entry());
}
