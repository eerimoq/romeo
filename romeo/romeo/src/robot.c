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
 * This file is part of the Romeo project.
 */

#include "simba.h"
#include "romeo.h"
#include "robot.h"

#define ROBOT_WHEEL_DISTANCE 0.2f
#define ROBOT_WHEEL_RADIUS   0.04f

/* Parameters. */
static int parameter_watchdog_enabled_value = 1;
static struct fs_parameter_t parameter_watchdog_enabled;

/* Counters. */
static struct fs_counter_t counter_tick;
static struct fs_counter_t counter_number_of_state_transitions;

static const char *state_fmt[] = {
    "idle",
    "starting",
    "cutting",
    "searching_for_base_station",
    "in_base_station"
};

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
            callback = robot_transition__idle__starting;
            break;
        default:
            break;
        }
        break;

    case ROBOT_STATE_STARTING:
        switch (next) {
        case ROBOT_STATE_CUTTING:
            callback = robot_transition__starting__cutting;
            break;
        case ROBOT_STATE_IN_BASE_STATION:
            callback = robot_transition__starting__in_base_station;
            break;
        default:
            break;
        }
        break;

    case ROBOT_STATE_CUTTING:
        switch (next) {
        case ROBOT_STATE_SEARCHING_FOR_BASE_STATION:
            callback = robot_transition__cutting__searching_for_base_station;
            break;
        case ROBOT_STATE_IDLE:
            callback = robot_transition__cutting__idle;
            break;
        default:
            break;
        }
        break;

    case ROBOT_STATE_SEARCHING_FOR_BASE_STATION:
        switch (next) {
        case ROBOT_STATE_IN_BASE_STATION:
            callback = robot_transition__searching_for_base_station__in_base_station;
            break;
        case ROBOT_STATE_IDLE:
            callback = robot_transition__searching_for_base_station__idle;
            break;
        default:
            break;
        }
        break;

    case ROBOT_STATE_IN_BASE_STATION:
        switch (next) {
        case ROBOT_STATE_CUTTING:
            callback = robot_transition__in_base_station__cutting;
            break;
        case ROBOT_STATE_IDLE:
            callback = robot_transition__in_base_station__idle;
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }

    /* Bad state transition. */
    if (callback == NULL) {
        log_object_print(NULL,
                         LOG_ERR,
                         FSTR("bad state transistion %d -> %d\r\n"),
                         current,
                         next);
        return (-1);
    }

    log_object_print(NULL,
                     LOG_NOTICE,
                     FSTR("state transistion %s -> %s\r\n"),
                     state_fmt[current],
                     state_fmt[next]);

    /* Call the transition callback. */
    state_callback = callback(robot_p);
    if (state_callback == NULL) {
        log_object_print(NULL,
                         LOG_NOTICE,
                         FSTR("failed state transistion %d -> %d\r\n"),
                         current,
                         next);
        return (-1);
    }

    /* Update state on successful state transition. */
    robot_p->state.current = next;
    robot_p->state.callback = state_callback;

    fs_counter_increment(&counter_number_of_state_transitions, 1);

    return (0);
}

int robot_module_init(void)
{
    fs_parameter_init(&parameter_watchdog_enabled,
                      FSTR("/robot/parameters/watchdog/enabled"),
                      fs_cmd_parameter_int,
                      &parameter_watchdog_enabled_value);
    fs_parameter_register(&parameter_watchdog_enabled);

    fs_counter_init(&counter_tick,
                    FSTR("/robot/counters/tick"),
                    0);
    fs_counter_register(&counter_tick);

    fs_counter_init(&counter_number_of_state_transitions,
                    FSTR("/robot/counters/number_of_state_transitions"),
                    0);
    fs_counter_register(&counter_number_of_state_transitions);

    robot_states_module_init();

    return (0);
}

int robot_init(struct robot_t *robot_p)
{
    robot_p->state.current = ROBOT_STATE_IDLE;
    robot_p->state.next = ROBOT_STATE_IDLE;
    robot_p->state.callback = robot_state_idle;
    robot_p->mode = ROBOT_MODE_AUTOMATIC;

    perimeter_wire_rx_init(&robot_p->perimeter,
                           &adc_0_dev,
                           &pin_a0_dev);

    battery_module_init();
    battery_init(&robot_p->battery,
                 &adc_0_dev,
                 &pin_a1_dev);

    movement_module_init();
    movement_init(&robot_p->movement,
                  ROBOT_WHEEL_DISTANCE,
                  ROBOT_WHEEL_RADIUS);

    motor_init(&robot_p->left_motor,
               &pin_d2_dev,
               &pin_d3_dev,
               &pwm_d10_dev,
               &adc_0_dev,
               &pin_a2_dev);

    motor_init(&robot_p->right_motor,
               &pin_d5_dev,
               &pin_d6_dev,
               &pwm_d11_dev,
               &adc_0_dev,
               &pin_a3_dev);

    watchdog_init(&robot_p->watchdog,
                  WATCHDOG_TIMEOUT_TICKS);

    /* Start convertion of sensor data. */
    perimeter_wire_rx_async_convert(&robot_p->perimeter);
    battery_async_convert(&robot_p->battery);
    motor_async_convert(&robot_p->left_motor);
    motor_async_convert(&robot_p->right_motor);

    return (0);
}

int robot_start(struct robot_t *robot_p)
{
    robot_p->state.next = ROBOT_STATE_STARTING;

    watchdog_start(&robot_p->watchdog);

    return (0);
}

int robot_stop(struct robot_t *robot_p)
{
    robot_p->state.next = ROBOT_STATE_IDLE;

    watchdog_stop(&robot_p->watchdog);

    return (0);
}

int robot_tick(struct robot_t *robot_p)
{
    fs_counter_increment(&counter_tick, 1);

    /* Stop the robot if the watchdog is enabled and has not been
       kicked recently. */
    if (parameter_watchdog_enabled_value == 1) {
        if (watchdog_tick(&robot_p->watchdog) == 0) {
            robot_p->state.next = ROBOT_STATE_IDLE;
        }
    }

    /* Read sensor data and update the sensor objects. */
    perimeter_wire_rx_async_wait(&robot_p->perimeter);
    perimeter_wire_rx_update(&robot_p->perimeter);

    battery_async_wait(&robot_p->battery);
    battery_update(&robot_p->battery);

    motor_async_wait(&robot_p->left_motor);
    motor_async_wait(&robot_p->right_motor);
    motor_update(&robot_p->left_motor);
    motor_update(&robot_p->right_motor);

    /* Start next convertion of sensor data. */
    perimeter_wire_rx_async_convert(&robot_p->perimeter);
    battery_async_convert(&robot_p->battery);
    motor_async_convert(&robot_p->left_motor);
    motor_async_convert(&robot_p->right_motor);

    log_object_print(NULL,
                     LOG_INFO,
                     FSTR("current state: %d\r\n"),
                     robot_p->state.current);

    /* Execute robot state machine. */
    if (robot_p->state.current == robot_p->state.next) {
        return (robot_p->state.callback(robot_p));
    } else {
        return (handle_state_transition(robot_p));
    }
}

int robot_watchdog_kick(struct robot_t *robot_p)
{
    return (watchdog_kick(&robot_p->watchdog));
}
