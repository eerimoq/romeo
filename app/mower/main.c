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

FS_COMMAND("/mower/mode/set", cmd_mower_mode_set);
FS_COMMAND("/mower/manual/movement/set", cmd_mower_control_movement_set);

FS_COUNTER(mower_processing_ticks);

#define MOWER_MODE_MANUAL    0
#define MOWER_MODE_AUTOMATIC 1

struct mower_t {
    volatile int mode;
    volatile struct {
        int speed;
        int omega;
    } manual;
    struct timer_t ticker;
    struct movement_t movement;
};

static struct mower_t mower;

static char shell_stack[450];

int cmd_mower_mode_set(int argc,
                       const char *argv[],
                       void *out_p,
                       void *in_p,
                       char *name_p)
{
    UNUSED(in_p);
    UNUSED(name_p);

    if (argc != 2) {
        std_fprintf(out_p, FSTR("Argumnts: {manual,automatic}\r\n"));
        return (1);
    }

    if (std_strcmp(argv[1], FSTR("manual")) == 0) {
        mower.mode = MOWER_MODE_MANUAL;
    } else if (std_strcmp(argv[1], FSTR("automatic")) == 0) {
        mower.mode = MOWER_MODE_AUTOMATIC;
    } else {
        std_fprintf(out_p, FSTR("Bad mode '%s'\r\n"), argv[1]);
    }

    return (0);
}

int cmd_mower_control_movement_set(int argc,
                                   const char *argv[],
                                   void *out_p,
                                   void *in_p,
                                   char *name_p)
{
    UNUSED(in_p);
    UNUSED(name_p);

    long speed;
    long omega;

    std_strtol(argv[1], &speed);
    std_strtol(argv[2], &omega);

    mower.manual.speed = speed;
    mower.manual.omega = omega;

    return (0);
}

static void timer_callback(void *arg_p)
{
    struct thrd_t *thrd_p = arg_p;

    thrd_resume_irq(thrd_p, 0);
}

static struct uart_driver_t uart;
static char qinbuf[32];
static struct shell_args_t shell_args;
static struct perimiter_wire_rx_t perimeter;
static struct thrd_t *self_p;

static void mower_init()
{
    struct time_t timeout;

    sys_start();
    uart_module_init();

    /* Setup UART. */
    uart_init(&uart, &uart_device[0], 38400, qinbuf, sizeof(qinbuf));
    uart_start(&uart);
    sys_set_stdout(&uart.chout);

    self_p = thrd_self();
    thrd_set_name("mower");

    std_printk(STD_LOG_NOTICE, FSTR("Starting mower"));

    mower.mode = MOWER_MODE_AUTOMATIC;
    movement_init(&mower.movement,
                  &pin_d2_dev,
                  &pin_d3_dev,
                  &pin_d4_dev,
                  &pin_d5_dev,
                  &pin_d6_dev,
                  &pin_d7_dev);

    perimiter_wire_rx_init(&perimeter, NULL, NULL);
    perimiter_wire_rx_start(&perimeter);

    /* Start the robot periodic timer with a 50ms period. */
    timeout.seconds = 0;
    timeout.nanoseconds = 50000000L;

    timer_set(&mower.ticker,
              &timeout,
              timer_callback,
              self_p,
              TIMER_PERIODIC);

    /* Start the shell. */
    shell_args.chin_p = &uart.chin;
    shell_args.chout_p = &uart.chout;
    thrd_spawn(shell_entry,
               &shell_args,
               20,
               shell_stack,
               sizeof(shell_stack));
}

int main()
{
    mower_init();

    /* Mower main loop. */
    while (1) {
        /* Resume from timer callback. */
        thrd_suspend(NULL);

        std_printk(STD_LOG_NOTICE, FSTR("Starting tick processing"));
        FS_COUNTER_INC(mower_processing_ticks, 1);

        if (mower.mode == MOWER_MODE_MANUAL) {
            movement_set(&mower.movement,
                         mower.manual.speed,
                         mower.manual.omega);
        } else {
        }

        std_printk(STD_LOG_NOTICE, FSTR("Tick processing finished"));
    }

    return (0);
}
