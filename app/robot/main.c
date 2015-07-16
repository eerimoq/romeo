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

FS_COMMAND("/robot/mode/set", cmd_robot_mode_set);
FS_COMMAND("/robot/manual/movement/set", cmd_robot_control_movement_set);

FS_COUNTER(robot_processing_ticks);

#define ROBOT_MODE_MANUAL    0
#define ROBOT_MODE_AUTOMATIC 1

struct motor_t {
    struct pin_driver_t in1;
    struct pin_driver_t in2;
    struct pin_driver_t enable;
};

struct robot_t {
    volatile int mode;
    volatile struct {
        int speed;
        int omega;
    } manual;
    struct timer_t ticker;
    struct motor_t left_motor;
    struct motor_t right_motor;
};

static struct robot_t robot;

static char shell_stack[450];

int cmd_robot_mode_set(int argc,
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
        robot.mode = ROBOT_MODE_MANUAL;
    } else if (std_strcmp(argv[1], FSTR("automatic")) == 0) {
        robot.mode = ROBOT_MODE_AUTOMATIC;
    } else {
        std_fprintf(out_p, FSTR("Bad mode '%s'\r\n"), argv[1]);
    }

    return (0);
}

int cmd_robot_control_movement_set(int argc,
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

    robot.manual.speed = speed;
    robot.manual.omega = omega;

    return (0);
}

static void timer_callback(void *arg_p)
{
    struct thrd_t *thrd_p = arg_p;

    thrd_resume_irq(thrd_p, 0);
}

static int motor_init(struct motor_t *motor_p,
                      struct pin_device_t *in1_p,
                      struct pin_device_t *in2_p,
                      struct pin_device_t *enable_p)
{
    /* Initialize all pins connected to the motor controllers. */
    pin_init(&motor_p->in1, in1_p, PIN_OUTPUT);
    pin_init(&motor_p->in2, in2_p, PIN_OUTPUT);
    pin_init(&motor_p->enable, enable_p, PIN_OUTPUT);

    return (0);
}

static int motor_set_omega(struct motor_t *motor_p,
                           int omega)
{
    /* Rotation direction pins. */
    if (omega > 0) {
        pin_write(&motor_p->in1, 1);
        pin_write(&motor_p->in2, 0);
    } else {
        pin_write(&motor_p->in1, 0);
        pin_write(&motor_p->in2, 1);
    }

    /* PWM pin. */
    pin_write(&motor_p->enable, 1);

    return (0);
}

static struct uart_driver_t uart;
static char qinbuf[32];
static struct shell_args_t shell_args;
static struct perimiter_wire_rx_t perimeter;
static struct thrd_t *self_p;

static void robot_init()
{
    struct time_t timeout;

    sys_start();
    uart_module_init();

    /* Setup UART. */
    uart_init(&uart, &uart_device[0], 38400, qinbuf, sizeof(qinbuf));
    uart_start(&uart);
    sys_set_stdout(&uart.chout);

    self_p = thrd_self();
    thrd_set_name("robot");

    std_printk(STD_LOG_NOTICE, FSTR("Starting robot"));

    robot.mode = ROBOT_MODE_AUTOMATIC;
    motor_init(&robot.left_motor,
               &pin_d2_dev,
               &pin_d3_dev,
               &pin_d4_dev);
    motor_init(&robot.right_motor,
               &pin_d5_dev,
               &pin_d6_dev,
               &pin_d7_dev);

    perimiter_wire_rx_init(&perimeter, NULL, NULL);
    perimiter_wire_rx_start(&perimeter);

    /* Start the robot periodic timer with a 50ms period. */
    timeout.seconds = 0;
    timeout.nanoseconds = 50000000L;

    timer_set(&robot.ticker,
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
    float left_wheel_omega;
    float right_wheel_omega;
    int speed = 0;
    int omega = 0;

    robot_init();

    /* Robot main loop. */
    while (1) {
        /* Resume from timer callback. */
        thrd_suspend(NULL);

        std_printk(STD_LOG_NOTICE, FSTR("Starting tick processing"));
        FS_COUNTER_INC(robot_processing_ticks, 1);

        if (robot.mode == ROBOT_MODE_MANUAL) {
            speed = robot.manual.speed;
            omega = robot.manual.omega;
        } else {
        }

        movement_calculate_wheels_omega(speed,
                                        omega,
                                        &left_wheel_omega,
                                        &right_wheel_omega);
        motor_set_omega(&robot.left_motor, left_wheel_omega);
        motor_set_omega(&robot.right_motor, right_wheel_omega);

        std_printk(STD_LOG_NOTICE,
                   FSTR("left_wheel_omega %d, right_wheel_omega %d [rad/100*s]"),
                   (int)left_wheel_omega,
                   (int)right_wheel_omega);

        std_printk(STD_LOG_NOTICE, FSTR("Tick processing finished"));
    }

    return (0);
}
