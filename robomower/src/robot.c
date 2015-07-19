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

FS_COMMAND("/robot/status", robot_cmd_status);
FS_COMMAND("/robot/start", robot_cmd_start);
FS_COMMAND("/robot/stop", robot_cmd_stop);
FS_COMMAND("/robot/mode/set", robot_cmd_mode_set);
FS_COMMAND("/robot/manual/movement/set", robot_cmd_manual_movement_set);

FS_COUNTER(robot_process);
FS_COUNTER(robot_state_idle);
FS_COUNTER(robot_state_starting);
FS_COUNTER(robot_state_cutting);
FS_COUNTER(robot_state_searching_for_base_station);
FS_COUNTER(robot_state_in_base_station);
FS_COUNTER(number_of_state_changes);

FS_COUNTER(robot_cutting_state_forward);
FS_COUNTER(robot_cutting_state_backwards);
FS_COUNTER(robot_cutting_state_rotating);

FS_COUNTER(robot_odometer);

/* The processing loop period in milliseconds. */
#define PROCESS_PERIOD_MS 50L

/* Robot modes. */
#define ROBOT_MODE_MANUAL    0
#define ROBOT_MODE_AUTOMATIC 1

/* Robot states. */
#define ROBOT_STATE_IDLE                       0
#define ROBOT_STATE_STARTING                   1
#define ROBOT_STATE_CUTTING                    2
#define ROBOT_STATE_SEARCHING_FOR_BASE_STATION 3
#define ROBOT_STATE_IN_BASE_STATION            4

/* Cutting states. */
#define CUTTING_STATE_FORWARD   0
#define CUTTING_STATE_BACKWARDS 1
#define CUTTING_STATE_ROTATING  2

/* Number of ticks in cutting states. */
#ifndef CUTTING_STATE_BACKWARDS_TICKS
#    define CUTTING_STATE_BACKWARDS_TICKS 40
#endif
#ifndef CUTTING_STATE_ROTATING_TICKS
#    define CUTTING_STATE_ROTATING_TICKS  40
#endif

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
    struct {
        int state;
        int ticks_left;
    } cutting;
    volatile struct {
        float speed;
        float omega;
    } manual;
    struct thrd_t *self_p;
    struct timer_t ticker;
    struct motor_t left_motor;
    struct motor_t right_motor;
    struct perimeter_wire_rx_t perimeter;
};

static struct robot_t robot;

/* Modes as strings. */
static FAR const char manual_string[] = "manual";
static FAR const char automatic_string[] = "automatic";

FAR const char FAR *mode_as_string[] = {
    manual_string,
    automatic_string
};

/* States as strings. */
static FAR const char idle_string[] = "idle";
static FAR const char starting_string[] = "starting";
static FAR const char cutting_string[] = "cutting";
static FAR const char searching_for_base_station_string[] = "searching_for_base_station";
static FAR const char in_base_station_string[] = "in_base_station";

FAR const char FAR *state_as_string[] = {
    idle_string,
    starting_string,
    cutting_string,
    searching_for_base_station_string,
    in_base_station_string
};

/* Cutting states as strings. */
static FAR const char cutting_forward_string[] = "forward";
static FAR const char cutting_backwards_string[] = "backwards";
static FAR const char cutting_rotating_string[] = "rotating";

FAR const char FAR *cutting_state_as_string[] = {
    cutting_forward_string,
    cutting_backwards_string,
    cutting_rotating_string
};

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

int robot_cmd_start(int argc,
                    const char *argv[],
                    void *out_p,
                    void *in_p)
{
    robot.state.next = ROBOT_STATE_STARTING;

    return (0);
}

int robot_cmd_stop(int argc,
                   const char *argv[],
                   void *out_p,
                   void *in_p)
{
    robot.state.next = ROBOT_STATE_IDLE;

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

    std_fprintf(out_p, FSTR("mode = "));
    std_fprintf(out_p, mode_as_string[robot.mode]);
    std_fprintf(out_p, FSTR("\r\nstate = "));
    std_fprintf(out_p, state_as_string[robot.state.current]);
    std_fprintf(out_p, FSTR("\r\ncutting.state = "));
    std_fprintf(out_p, cutting_state_as_string[robot.cutting.state]);
    std_fprintf(out_p,
                FSTR("\r\nprocess period = %ld ms\r\n"),
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

static int state_idle(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_state_idle, 1);

    return (0);
}

static int state_starting(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_state_starting, 1);

    robot_p->state.next = ROBOT_STATE_CUTTING;

    return (0);
}

static int cutting_manual(struct robot_t *robot_p,
                          float *speed,
                          float *omega)
{
    *speed = robot_p->manual.speed;
    *omega = robot_p->manual.omega;

    return (0);
}

static int is_time_to_search_for_base_station(struct robot_t *robot_p)
{
    return (0);
}

static int is_inside_perimeter_wire(float signal)
{
    return (signal >= 0.0f);
}

static int cutting_automatic(struct robot_t *robot_p,
                             float *speed,
                             float *omega)
{
    float signal;

    /* Default no movement. */
    *speed = 0.0f;
    *omega = 0.0f;

    /* Search for base station if battery voltage is low. */
    if (is_time_to_search_for_base_station(robot_p)) {
        robot_p->state.next = ROBOT_STATE_SEARCHING_FOR_BASE_STATION;
    } else {
        robot_p->cutting.ticks_left--;

        switch (robot_p->cutting.state) {

        case CUTTING_STATE_FORWARD:
            FS_COUNTER_INC(robot_cutting_state_forward, 1);

            signal = perimeter_wire_rx_get_signal(&robot_p->perimeter);

            if (is_inside_perimeter_wire(signal)) {
                *speed = 0.1f;
                *omega = 0.0f;
            } else {
                /* Enter backwards state. */
                robot_p->cutting.ticks_left = CUTTING_STATE_BACKWARDS_TICKS;
                robot_p->cutting.state = CUTTING_STATE_BACKWARDS;
            }
            break;

        case CUTTING_STATE_BACKWARDS:
            FS_COUNTER_INC(robot_cutting_state_backwards, 1);

            *speed = -0.1f;
            *omega = 0.0f;

            if (robot_p->cutting.ticks_left == 0) {
                /* Enter rotating state. */
                /* TODO: number of ticks should be random. */
                *speed = 0.0f;
                robot_p->cutting.ticks_left = CUTTING_STATE_ROTATING_TICKS;
                robot_p->cutting.state = CUTTING_STATE_ROTATING;
            }
            break;

        case CUTTING_STATE_ROTATING:
            FS_COUNTER_INC(robot_cutting_state_rotating, 1);

            *speed = 0.0f;
            *omega = 0.1f;

            if (robot_p->cutting.ticks_left == 0) {
                /* Enter forward state. */
                *omega = 0.0f;
                robot_p->cutting.state = CUTTING_STATE_FORWARD;
            }
            break;
        }
    }

    return (0);
}

static int state_cutting(struct robot_t *robot_p)
{
    float left_wheel_omega;
    float right_wheel_omega;
    float speed;
    float omega;

    FS_COUNTER_INC(robot_state_cutting, 1);

    /* Calculate new motor speeds. */
    if (robot_p->mode == ROBOT_MODE_MANUAL) {
        cutting_manual(robot_p, &speed, &omega);
    } else {
        cutting_automatic(robot_p, &speed, &omega);
    }

    /* Calculate new driver motor speeds and set them. */
    movement_calculate_wheels_omega(speed,
                                    omega,
                                    &left_wheel_omega,
                                    &right_wheel_omega);

    motor_set_omega(&robot_p->left_motor, left_wheel_omega);
    motor_set_omega(&robot_p->right_motor, right_wheel_omega);

    FS_COUNTER_INC(robot_odometer, speed * PROCESS_PERIOD_MS);

    return (0);
}

static int state_searching_for_base_station(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_state_searching_for_base_station, 1);

    return (0);
}

static int state_in_base_station(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_state_in_base_station, 1);

    return (0);
}

static state_callback_t transition__idle__starting(struct robot_t *robot_p)
{
    return (state_starting);
}

static state_callback_t transition__starting__cutting(struct robot_t *robot_p)
{
    return (state_cutting);
}

static state_callback_t transition__starting__in_base_station(struct robot_t *robot_p)
{
    return (state_in_base_station);
}

static state_callback_t transition__cutting__idle(struct robot_t *robot_p)
{
    return (state_idle);
}

static state_callback_t transition__cutting__searching_for_base_station(struct robot_t *robot_p)
{
    return (state_searching_for_base_station);
}

static state_callback_t transition__searching_for_base_station__idle(struct robot_t *robot_p)
{
    return (state_idle);
}

static state_callback_t transition__in_base_station__idle(struct robot_t *robot_p)
{
    return (state_idle);
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

    case ROBOT_STATE_IDLE:
        switch (next) {
        case ROBOT_STATE_STARTING:
            callback = transition__idle__starting;
            break;
        default:
            break;
        }

    case ROBOT_STATE_STARTING:
        switch (next) {
        case ROBOT_STATE_CUTTING:
            callback = transition__starting__cutting;
            break;
        case ROBOT_STATE_IN_BASE_STATION:
            callback = transition__starting__in_base_station;
            break;
        default:
            break;
        }

    case ROBOT_STATE_CUTTING:
        switch (next) {
        case ROBOT_STATE_SEARCHING_FOR_BASE_STATION:
            callback = transition__cutting__searching_for_base_station;
            break;
        case ROBOT_STATE_IDLE:
            callback = transition__cutting__idle;
            break;
        default:
            break;
        }

    case ROBOT_STATE_SEARCHING_FOR_BASE_STATION:
        switch (next) {
        case ROBOT_STATE_IDLE:
            callback = transition__searching_for_base_station__idle;
            break;
        default:
            break;
        }

    case ROBOT_STATE_IN_BASE_STATION:
        switch (next) {
        case ROBOT_STATE_IDLE:
            callback = transition__in_base_station__idle;
            break;
        default:
            break;
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

int robot_init()
{
    robot.state.current = ROBOT_STATE_IDLE;
    robot.state.next = ROBOT_STATE_IDLE;
    robot.state.callback = state_idle;
    robot.mode = ROBOT_MODE_AUTOMATIC;

    robot.cutting.state = CUTTING_STATE_FORWARD;

    motor_init(&robot.left_motor,
               &pin_d2_dev,
               &pin_d3_dev,
               &pwm_d10_dev);
    motor_init(&robot.right_motor,
               &pin_d5_dev,
               &pin_d6_dev,
               &pwm_d11_dev);

    perimeter_wire_rx_init(&robot.perimeter, NULL, NULL);

    return (0);
}

void *robot_entry(void *arg_p)
{
    struct time_t timeout;

    robot.self_p = thrd_self();
    thrd_set_name("robot");

    /* Start the robot periodic timer with a 50ms period. */
    timeout.seconds = 0;
    timeout.nanoseconds = PROCESS_PERIOD_MS * 1000000L;

    timer_set(&robot.ticker,
              &timeout,
              timer_callback,
              robot.self_p,
              TIMER_PERIODIC);

    /* Robot main loop. */
    while (1) {
        /* Timer callback resumes this thread. */
        thrd_suspend(NULL);

        robot_process(&robot);
    }

    return (0);
}

int robot_manual_start()
{
    robot.state.next = ROBOT_STATE_STARTING;

    return (0);
}
