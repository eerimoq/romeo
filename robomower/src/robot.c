/**
 * @file robot.c
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

FS_COMMAND("/robot/mode/set", robot_cmd_mode_set);
FS_COMMAND("/robot/manual/state/set", robot_cmd_manual_state_set);
FS_COMMAND("/robot/manual/movement/set", robot_cmd_manual_movement_set);
FS_COMMAND("/robot/status", robot_cmd_status);

FS_COUNTER(robot_process);
FS_COUNTER(robot_process_state_on);
FS_COUNTER(robot_process_state_off);
FS_COUNTER(number_of_state_changes);

/* The processing loop period in milliseconds. */
#define PROCESS_PERIOD_MS 50L

/* Robot modes. */
#define ROBOT_MODE_MANUAL    0
#define ROBOT_MODE_AUTOMATIC 1

/* Robot state. */
#define ROBOT_STATE_OFF   0
#define ROBOT_STATE_ON    1

struct robot_t;

typedef int (*state_callback_t)(struct robot_t *);
typedef state_callback_t (*transition_callback_t)(struct robot_t *);

struct robot_state_t {
    volatile int current;
    volatile int next;
    int (*callback)(struct robot_t *);
};

struct robot_t {
    volatile int mode;
    struct robot_state_t state;
    volatile struct {
        float speed;
        float omega;
    } manual;
    struct timer_t ticker;
    struct motor_t left_motor;
    struct motor_t right_motor;
    struct perimiter_wire_rx_t perimeter;
};

static struct robot_t robot;
static struct thrd_t *self_p;

int robot_cmd_mode_set(int argc,
                       const char *argv[],
                       void *out_p,
                       void *in_p)
{
    UNUSED(in_p);

    if (argc != 2) {
        std_fprintf(out_p, FSTR("Usage: set {manual,automatic}\r\n"));
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

int robot_cmd_manual_state_set(int argc,
                               const char *argv[],
                               void *out_p,
                               void *in_p)
{
    UNUSED(in_p);

    int next;

    if (argc != 2) {
        std_fprintf(out_p, FSTR("Usage: set {on,off}\r\n"));
        return (1);
    }

    if (std_strcmp(argv[1], FSTR("on")) == 0) {
        next = ROBOT_STATE_ON;
    } else if (std_strcmp(argv[1], FSTR("off")) == 0) {
        next = ROBOT_STATE_OFF;
    } else {
        std_fprintf(out_p, FSTR("Bad state '%s'\r\n"), argv[1]);
        return (1);
    }

    robot.state.next = next;

    return (0);
}

int robot_cmd_manual_movement_set(int argc,
                                  const char *argv[],
                                  void *out_p,
                                  void *in_p)
{
    UNUSED(in_p);

    long speed;
    long omega;

    std_strtol(argv[1], &speed);
    std_strtol(argv[2], &omega);

    robot.manual.speed = ((float)speed) / 100.0f;
    robot.manual.omega = ((float)omega) / 100.0f;

    return (0);
}

int robot_cmd_status(int argc,
                     const char *argv[],
                     void *out_p,
                     void *in_p)
{
    UNUSED(in_p);

    std_fprintf(out_p, FSTR("mode = %d (0=manual, 1=automatic)\r\n"
                            "state = %d (0=off, 1=on)\r\n"
                            "process period = %ld ms\r\n"),
                robot.mode,
                robot.state.current,
                PROCESS_PERIOD_MS);

    if (robot.mode == ROBOT_MODE_MANUAL) {
        std_fprintf(out_p,
                    FSTR("speed = %d.%u m/s\r\n"
                         "omega = %d.%u rad/s\r\n"),
                    (int)(robot.manual.speed),
                    ((unsigned int)(robot.manual.speed * 100.0f)) % 100,
                    (int)(robot.manual.omega),
                    ((unsigned int)(robot.manual.omega * 100.0f)) % 100);
    } else {
    }

    return (0);
}

static void timer_callback(void *arg_p)
{
    struct thrd_t *thrd_p = arg_p;

    thrd_resume_irq(thrd_p, 0);
}

static int robot_start(struct robot_t *robot_p)
{
    std_printk(STD_LOG_NOTICE, FSTR("starting robot"));

    robot_p->state.next = ROBOT_STATE_ON;

    perimiter_wire_rx_start(&robot_p->perimeter);

    return (0);
}

static int robot_stop(struct robot_t *robot_p)
{
    std_printk(STD_LOG_NOTICE, FSTR("stopping robot"));

    robot_p->state.next = ROBOT_STATE_OFF;

    //perimiter_wire_rx_stop(&robot_p->perimeter);

    return (0);
}

static int state_on(struct robot_t *robot_p)
{
    float left_wheel_omega;
    float right_wheel_omega;
    float speed;
    float omega;

    FS_COUNTER_INC(robot_process_state_on, 1);

    if (robot_p->mode == ROBOT_MODE_MANUAL) {
        speed = robot_p->manual.speed;
        omega = robot_p->manual.omega;
    } else {
        /* Rotate the robot. */
        speed = 0.0f;
        omega = 0.1f;
    }

    /* Calculate new driver motor speeds and set them. */
    movement_calculate_wheels_omega(speed,
                                    omega,
                                    &left_wheel_omega,
                                    &right_wheel_omega);
    motor_set_omega(&robot_p->left_motor, left_wheel_omega);
    motor_set_omega(&robot_p->right_motor, right_wheel_omega);

    return (0);
}

static int state_off(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_process_state_off, 1);

    return (0);
}

static state_callback_t transition_off_on(struct robot_t *robot_p)
{
    if (robot_start(robot_p) != 0) {
        return (NULL);
    }

    return (state_on);
}

static state_callback_t transition_on_off(struct robot_t *robot_p)
{
    if (robot_stop(robot_p) != 0) {
        return (NULL);
    }

    return (state_off);
}

/**
 * Handle state transition.
 * @return zero(0) or negative error code
 */
static int handle_state_transition(struct robot_t *robot_p)
{
    int current = robot_p->state.current;
    int next = robot_p->state.next;
    transition_callback_t callback;
    state_callback_t state_callback;

    callback = NULL;

    switch (current) {

    case ROBOT_STATE_OFF:
        switch (next) {
        case ROBOT_STATE_ON: callback = transition_off_on; break;
        default: break;
        }

    case ROBOT_STATE_ON:
        switch (next) {
        case ROBOT_STATE_OFF: callback = transition_on_off; break;
        default: break;
        }

    default:
        break;
    }

    /* Bad state transition. */
    if (callback == NULL) {
        std_printk(STD_LOG_ERR,
                   FSTR("bad state transistion %d -> %d"),
                   current,
                   next);
        return (-1);
    }

    std_printk(STD_LOG_NOTICE,
               FSTR("state transistion %d -> %d"),
               current,
               next);

    /* Call the transition callback. */
    state_callback = callback(robot_p);
    if (state_callback == NULL) {
        std_printk(STD_LOG_NOTICE,
                   FSTR("failed state transistion %d -> %d"),
                   current,
                   next);
        return (-1);
    }

    /* Update state on successful state transition. */
    robot_p->state.current = next;
    robot_p->state.callback = state_callback;

    FS_COUNTER_INC(number_of_state_changes, 1);

    return (0);
}

static int robot_process(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_process, 1);

    if (robot_p->state.current == robot_p->state.next) {
        return (robot_p->state.callback(robot_p));
    } else {
        return (handle_state_transition(robot_p));
    }
}

static int robot_init(struct robot_t *robot_p)
{
    struct time_t timeout;

    self_p = thrd_self();
    thrd_set_name("robot");

    std_printk(STD_LOG_NOTICE, FSTR("initializing robot"));

    robot_p->state.current = ROBOT_STATE_OFF;
    robot_p->state.next = ROBOT_STATE_OFF;
    robot_p->state.callback = state_off;
    robot_p->mode = ROBOT_MODE_AUTOMATIC;

    motor_init(&robot_p->left_motor,
               &pin_d2_dev,
               &pin_d3_dev,
               &pwm_d10_dev);
    motor_init(&robot_p->right_motor,
               &pin_d5_dev,
               &pin_d6_dev,
               &pwm_d11_dev);

    perimiter_wire_rx_init(&robot_p->perimeter, NULL, NULL);

    /* Start the robot periodic timer with a 50ms period. */
    timeout.seconds = 0;
    timeout.nanoseconds = PROCESS_PERIOD_MS * 1000000L;

    timer_set(&robot_p->ticker,
              &timeout,
              timer_callback,
              self_p,
              TIMER_PERIODIC);

    return (0);
}

int robot_entry()
{
    robot_init(&robot);

    /* Robot main loop. */
    while (1) {
        /* Timer callback resumes this thread. */
        thrd_suspend(NULL);

        robot_process(&robot);
    }

    return (0);
}
