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

#define VERSION_STR "0.1.0"

static struct uart_driver_t uart;
static char qinbuf[32];
static struct shell_args_t shell_args;

int main()
{
    struct perimeter_wire_tx_t perimeter_wire;

    sys_start();
    uart_module_init();

    /* Setup UART. */
    uart_init(&uart, &uart_device[0], 38400, qinbuf, sizeof(qinbuf));
    uart_start(&uart);
    sys_set_stdout(&uart.chout);

    std_printf(FSTR("RoboMower base station v" VERSION_STR "\r\n"));

    /* Start transmitting the signal on the perimeter wire. */
    perimeter_wire_tx_module_init();
    perimeter_wire_tx_init(&perimeter_wire,
                           &pin_d7_dev,
                           &pin_d8_dev);
    perimeter_wire_tx_start(&perimeter_wire);

    /* Start the shell. */
    shell_args.chin_p = &uart.chin;
    shell_args.chout_p = &uart.chout;

    /* The shell never returns. */
    shell_entry(&shell_args);

    return (0);
}
