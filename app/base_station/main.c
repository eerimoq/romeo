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

FS_COMMAND_DEFINE("/base_station/status", base_station_cmd_status);

static struct uart_driver_t uart;
static char qinbuf[32];
static struct shell_args_t shell_args;

static struct base_station_t base_station;
static struct timer_t ticker;
static struct thrd_t *self_p;

static char shell_stack[350];

int base_station_cmd_status(int argc,
                            const char *argv[],
                            void *out_p,
                            void *in_p)
{
    UNUSED(in_p);

    std_fprintf(out_p,
                FSTR("perimeter wire current = %d.%u\r\n"),
		    (int)(base_station.perimeter.current),
                ((unsigned int)(base_station.perimeter.current * 100.0f)) % 100);

    return (0);
}

static void timer_callback(void *arg_p)
{
    struct thrd_t *thrd_p = arg_p;

    thrd_resume_isr(thrd_p, 0);
}

static int init()
{
    sys_start();
    uart_module_init();

    /* Setup UART. */
    uart_init(&uart, &uart_device[0], 38400, qinbuf, sizeof(qinbuf));
    uart_start(&uart);
    sys_set_stdout(&uart.chout);

    /* Sample */

    /* Start transmitting the signal on the perimeter wire. */
    perimeter_wire_tx_module_init();

    base_station_init(&base_station);

    self_p = thrd_self();
    thrd_set_name("base_station");

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
    struct time_t timeout;

    init();

    std_printf(FSTR("Romeo - base station version " VERSION_STR "\r\n"));

    /* Start the robot periodic timer with a 50ms period. */
    timeout.seconds = 0;
    timeout.nanoseconds = PROCESS_PERIOD_NS;

    timer_set(&ticker,
	      &timeout,
	      timer_callback,
	      self_p,
	      TIMER_PERIODIC);

    base_station_start(&base_station);

    /* Base station main loop. */
    while (1) {
	/* Timer callback resumes this thread. */
	thrd_suspend(NULL);
        base_station_tick(&base_station);
    }

    return (0);
}
