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
#include "robot.h"

#define SIGNAL_THRESHOLD_MAX 10.0f
#define SIGNAL_THRESHOLD_MIN -10.0f

FS_COUNTER(robot_state_idle);
FS_COUNTER(robot_state_starting);
FS_COUNTER(robot_state_cutting);
FS_COUNTER(robot_state_searching_for_base_station);
FS_COUNTER(robot_state_in_base_station);

FS_COUNTER(robot_cutting_state_forward);
FS_COUNTER(robot_cutting_state_backwards);
FS_COUNTER(robot_cutting_state_rotating);

FS_COUNTER(robot_odometer);

FS_COUNTER(robot_perimeter_no_signal);

static int is_time_to_search_for_base_station(struct robot_t *robot_p)
{
    return (power_get_stored_energy_level(&robot_p->power) <= 20);
}

static int is_inside_perimeter_wire(float signal)
{
    return (signal >= 0.0f);
}

static int cutting_manual(struct robot_t *robot_p,
                          float *speed,
                          float *omega)
{
    *speed = robot_p->manual.speed;
    *omega = robot_p->manual.omega;

    return (0);
}

static int cutting_automatic(struct robot_t *robot_p,
                             float *speed_p,
                             float *omega_p)
{
    float signal;
    struct cutting_state_t *cutting_p = &robot_p->substate.cutting;

    /* Default no movement. */
    *speed_p = 0.0f;
    *omega_p = 0.0f;

    /* Don't do anything if the perimeter wire signal cannot be found. */
    signal = perimeter_wire_rx_get_signal(&robot_p->perimeter);

    /* Check versus thresholds. */
    if ((signal < SIGNAL_THRESHOLD_MAX) && (signal > SIGNAL_THRESHOLD_MIN)) {
        FS_COUNTER_INC(robot_perimeter_no_signal, 1);
        return (0);
    }

    /* Search for base station if battery voltage is low. */
    if (is_time_to_search_for_base_station(robot_p)) {
        robot_p->state.next = ROBOT_STATE_SEARCHING_FOR_BASE_STATION;
    } else {
        cutting_p->ticks_left--;

        switch (cutting_p->state) {

        case CUTTING_STATE_FORWARD:
            FS_COUNTER_INC(robot_cutting_state_forward, 1);

            if (is_inside_perimeter_wire(signal)) {
                *speed_p = 0.1f;
                *omega_p = 0.0f;
            } else {
                /* Enter backwards state. */
                cutting_p->ticks_left = CUTTING_STATE_BACKWARDS_TICKS;
                cutting_p->state = CUTTING_STATE_BACKWARDS;
            }
            break;

        case CUTTING_STATE_BACKWARDS:
            FS_COUNTER_INC(robot_cutting_state_backwards, 1);

            *speed_p = -0.1f;
            *omega_p = 0.0f;

            if (cutting_p->ticks_left == 0) {
                /* Enter rotating state. */
                /* TODO: number of ticks should be random. */
                *speed_p = 0.0f;
                cutting_p->ticks_left = CUTTING_STATE_ROTATING_TICKS;
                cutting_p->state = CUTTING_STATE_ROTATING;
            }
            break;

        case CUTTING_STATE_ROTATING:
            FS_COUNTER_INC(robot_cutting_state_rotating, 1);

            *speed_p = 0.0f;
            *omega_p = 0.1f;

            if (cutting_p->ticks_left == 0) {
                /* Enter forward state. */
                *omega_p = 0.0f;
                cutting_p->state = CUTTING_STATE_FORWARD;
            }
            break;
        }
    }

    return (0);
}

int state_idle(struct robot_t *robot_p)
{
    float left_wheel_omega;
    float right_wheel_omega;

    FS_COUNTER_INC(robot_state_idle, 1);

    /* Robot standing still in idle state. */
    left_wheel_omega = 0.0f;
    right_wheel_omega = 0.0f;

    motor_set_omega(&robot_p->left_motor, left_wheel_omega);
    motor_set_omega(&robot_p->right_motor, right_wheel_omega);

    return (0);
}

int state_starting(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_state_starting, 1);

    /* Start sampling the perimeter wire signal. */
    perimeter_wire_rx_start(&robot_p->perimeter);
    power_start(&robot_p->power);

    robot_p->state.next = ROBOT_STATE_CUTTING;

    return (0);
}

int state_cutting(struct robot_t *robot_p)
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

    /* Only measure when driving forwards. */
    if (speed > 0.0f) {
        FS_COUNTER_INC(robot_odometer, speed * PROCESS_PERIOD_MS);
    }

    return (0);
}

int state_searching_for_base_station(struct robot_t *robot_p)
{
    float signal;
    float left_wheel_omega;
    float right_wheel_omega;
    struct searching_for_base_station_state_t *searching_p =
        &robot_p->substate.searching;

    FS_COUNTER_INC(robot_state_searching_for_base_station, 1);

    signal = perimeter_wire_rx_get_signal(&robot_p->perimeter);

    /* TODO: Implement searching and following algorithms. */
    if (searching_p->state == SEARCHING_STATE_SEARCHING_FOR_PERIMETER_WIRE) {
        /* Find the perimeter wire. */
        left_wheel_omega = 0.05f;
        right_wheel_omega = 0.05f;
        searching_p->state = SEARCHING_STATE_FOLLOWING_PERIMETER_WIRE;
    } else {
        /* Follow the perimeter wire to the base station. */
        left_wheel_omega = 0.0f;
        right_wheel_omega = 0.0f;

        /* When the robot is stuck and charging, it has arrived to the
           base station. */
        robot_p->state.next = ROBOT_STATE_IN_BASE_STATION;
    }

    /* Find the perimeter wire. */
    motor_set_omega(&robot_p->left_motor, left_wheel_omega);
    motor_set_omega(&robot_p->right_motor, right_wheel_omega);

    return (0);
}

int state_in_base_station(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_state_in_base_station, 1);

    /* Wait until plenty of energy is available. */
    if (power_get_stored_energy_level(&robot_p->power)
        == POWER_STORED_ENERGY_LEVEL_MAX) {
        robot_p->substate.cutting.ticks_left = CUTTING_STATE_BACKWARDS_TICKS;
        robot_p->substate.cutting.state = CUTTING_STATE_BACKWARDS;
        robot_p->state.next = ROBOT_STATE_CUTTING;
    }

    /* Stand still. */
    motor_set_omega(&robot_p->left_motor, 0.0f);
    motor_set_omega(&robot_p->right_motor, 0.0f);

    return (0);
}
