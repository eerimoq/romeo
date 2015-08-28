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
#include <math.h>

#define CONTROL_ROTATE_THRESHOLD 1.0f

FS_COUNTER_DEFINE("robot/counters/state_idle", robot_state_idle);
FS_COUNTER_DEFINE("robot/counters/state_starting", robot_state_starting);
FS_COUNTER_DEFINE("robot/counters/state_cutting", robot_state_cutting);
FS_COUNTER_DEFINE("robot/counters/state_searching_for_base_station", robot_state_searching_for_base_station);
FS_COUNTER_DEFINE("robot/counters/state_in_base_station", robot_state_in_base_station);

FS_COUNTER_DEFINE("robot/counters/cutting_state_forward", robot_cutting_state_forward);
FS_COUNTER_DEFINE("robot/counters/cutting_state_backwards", robot_cutting_state_backwards);
FS_COUNTER_DEFINE("robot/counters/cutting_state_rotating", robot_cutting_state_rotating);

FS_COUNTER_DEFINE("robot/counters/odometer", robot_odometer);

FS_COUNTER_DEFINE("robot/counters/is_Stuck", robot_is_stuck);

FS_COUNTER_DEFINE("robot/counters/is_inside_perimeter_wire", robot_is_inside_perimeter_wire);
FS_COUNTER_DEFINE("robot/counters/is_outside_perimeter_wire", robot_is_outside_perimeter_wire);

FS_PARAMETER_DEFINE("/robot/parameters/charging", robot_parameter_charging, int, 0);
FS_PARAMETER_DEFINE("/robot/parameters/search_for_the_base_station", robot_parameter_search_for_the_base_station, int, -1);

/**
 * @return true(1) if the robot should start searching for the base
 *         station, otherwise false(0).
 */
static int is_time_to_search_for_base_station(struct robot_t *robot_p)
{
    int search_for_the_base_station =
        FS_PARAMETER(robot_parameter_search_for_the_base_station);

    if (search_for_the_base_station != -1) {
        return (search_for_the_base_station != 0);
    }

    return (battery_get_stored_energy_level(&robot_p->battery) <= 20);
}

static int is_inside_perimeter_wire(float signal)
{
    int is_inside = (signal >= 3.0f);

    if (is_inside == 1) {
        FS_COUNTER_INC(robot_is_inside_perimeter_wire, 1);
    } else {
        FS_COUNTER_INC(robot_is_outside_perimeter_wire, 1);
    }

    return (is_inside);
}

static int is_stuck(struct robot_t *robot_p)
{
    int left_current;
    int right_current;

    left_current = motor_get_current(&robot_p->left_motor);
    right_current = motor_get_current(&robot_p->right_motor);

    if ((left_current > 500) || (right_current > 500)) {
        FS_COUNTER_INC(robot_is_stuck, 1);
        return (1);
    }

    return (0);
}

static int is_charging(struct robot_t *robot_p)
{
    return (FS_PARAMETER(robot_parameter_charging));
}

static int is_arriving_to_base_station(struct robot_t *robot_p)
{
    return (is_stuck(robot_p) && is_charging(robot_p));
}

/**
 * Follow the wire clockwise. The signal on the left hand side of the
 * robot should be negative, while the signal on the right hand side
 * should be positive.
 */
static int follow_perimeter_wire(struct robot_t *robot_p,
                                 float *left_wheel_omega_p,
                                 float *right_wheel_omega_p,
                                 float signal)
{
    float control;
    float speed, omega;

    /* Just wait if the robot is stuck. */
    if (is_stuck(robot_p)) {
        *left_wheel_omega_p = 0.0f;
        *right_wheel_omega_p = 0.0f;
        return (0);
    }

    /* Try to stay on top of the wire, hence set the actual value to
       0.0f. The returned control value is used to calculate motor
       speeds. */
    control = controller_pid_calculate(&robot_p->follow_pid_controller,
                                       0.0f,
                                       signal);

    std_printk(STD_LOG_INFO,
               FSTR("follow: signal = %d, control = %d"),
               (int)signal,
               (int)(100 * control));

    /* A big control value indicates the robot is off follow, so just
       rotate the robot. Start driving forward when the control value
       is sufficiantly small. */
    speed = 0.0f;

    if (control > CONTROL_ROTATE_THRESHOLD) {
        /* The robot is inside the perimeter wire, turn left towards
           the wire.*/
        omega = -0.2f;
    } else if (control < -CONTROL_ROTATE_THRESHOLD) {
        /* The robot is outside the perimeter wire, turn right towards
           the wire.*/
        omega = 0.2f;
    } else {
        /* Give the robot some speed forwards and try to follow the
           line. */
        speed = 0.1f;
        omega = -control / 4.0f;
    }

    /* Calculate new driver motor speeds and set them. */
    movement_calculate_wheels_omega(&robot_p->movement,
                                    speed,
                                    omega,
                                    left_wheel_omega_p,
                                    right_wheel_omega_p);

    return (0);
}

static int cutting_manual(struct robot_t *robot_p,
                          float *speed,
                          float *omega)
{
    if (!is_stuck(robot_p)) {
        *speed = robot_p->manual.speed;
        *omega = robot_p->manual.omega;
    } else {
        /* In manual mode, stand still when the robot is stuck. */
        *speed = 0.0f;
        *omega = 0.0f;
    }

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

    if (is_stuck(robot_p)) {
        /* TODO: try to get free */
        std_printk(STD_LOG_NOTICE, FSTR("cutting stuck"));
        return (0);
    }

    signal = perimeter_wire_rx_get_signal(&robot_p->perimeter);

    /* Search for base station if battery voltage is low. */
    if (is_time_to_search_for_base_station(robot_p)) {
        robot_p->state.next = ROBOT_STATE_SEARCHING_FOR_BASE_STATION;
    } else {
        cutting_p->ticks_left--;

        switch (cutting_p->state) {

        case CUTTING_STATE_FORWARD:
            FS_COUNTER_INC(robot_cutting_state_forward, 1);

            if (is_inside_perimeter_wire(signal)) {
                /* Just continue forward. */
                *speed_p = 0.1f;
                *omega_p = 0.0f;
            } else {
                /* Drive backwards when the perimerter wire is found. */
                cutting_p->ticks_left = CUTTING_STATE_BACKWARDS_TICKS;
                cutting_p->state = CUTTING_STATE_BACKWARDS;
            }
            break;

        case CUTTING_STATE_BACKWARDS:
            FS_COUNTER_INC(robot_cutting_state_backwards, 1);

            *speed_p = -0.1f;
            *omega_p = 0.0f;

            if (cutting_p->ticks_left == 0) {
                /* Enter rotating state and rotate a random number of
                   ticks. */
                *speed_p = 0.0f;
                cutting_p->ticks_left = CUTTING_STATE_ROTATING_TICKS;
                cutting_p->state = CUTTING_STATE_ROTATING;
            }
            break;

        case CUTTING_STATE_ROTATING:
            FS_COUNTER_INC(robot_cutting_state_rotating, 1);

            *speed_p = 0.0f;
            *omega_p = 0.4f;

            if (cutting_p->ticks_left == 0) {
                if (is_inside_perimeter_wire(signal)) {
                    cutting_p->state = CUTTING_STATE_FORWARD;
                } else {
                    /* Stop the robot if it is not inside the perimerter wire. */
                    *omega_p = 0.0f;
                    robot_p->state.next = ROBOT_STATE_IDLE;
                }
            }
            break;
        }
    }

    return (0);
}

int robot_state_idle(struct robot_t *robot_p)
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

int robot_state_starting(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_state_starting, 1);

    robot_p->state.next = ROBOT_STATE_CUTTING;

    return (0);
}

int robot_state_cutting(struct robot_t *robot_p)
{
    float left_wheel_omega;
    float right_wheel_omega;
    float speed;
    float omega;

    FS_COUNTER_INC(robot_state_cutting, 1);

    /* Calculate new robot speeds. */
    if (robot_p->mode == ROBOT_MODE_MANUAL) {
        cutting_manual(robot_p, &speed, &omega);
    } else {
        cutting_automatic(robot_p, &speed, &omega);
    }

    /* Convert the robot speeds to wheel motor angular velocities. */
    movement_calculate_wheels_omega(&robot_p->movement,
                                    speed,
                                    omega,
                                    &left_wheel_omega,
                                    &right_wheel_omega);

    /* Set the wheel motor angular velocities. */
    motor_set_omega(&robot_p->left_motor, left_wheel_omega);
    motor_set_omega(&robot_p->right_motor, right_wheel_omega);

    /* Only measure when driving forwards. */
    if (speed > 0.0f) {
        FS_COUNTER_INC(robot_odometer, speed * PROCESS_PERIOD_MS);
    }

    return (0);
}

int robot_state_searching_for_base_station(struct robot_t *robot_p)
{
    float signal;
    float left_wheel_omega;
    float right_wheel_omega;
    struct searching_for_base_station_state_t *searching_p =
        &robot_p->substate.searching;

    left_wheel_omega = 0.0f;
    right_wheel_omega = 0.0f;

    signal = perimeter_wire_rx_get_signal(&robot_p->perimeter);

    FS_COUNTER_INC(robot_state_searching_for_base_station, 1);

    if (searching_p->state == SEARCHING_STATE_SEARCHING_FOR_PERIMETER_WIRE) {
        /* No movement if the robot is stuck. */
        if (!is_stuck(robot_p)) {
            /* Find the perimeter wire. */
            if (is_inside_perimeter_wire(signal)) {
                left_wheel_omega = 0.05f;
                right_wheel_omega = 0.05f;
            } else {
                /* Perimeter wire found. */
                searching_p->state = SEARCHING_STATE_FOLLOWING_PERIMETER_WIRE;
            }
        } else {
            /* TODO: try to get free */
        }
    } else {
        /* Follow the perimeter wire to the base station. */
        if (!is_arriving_to_base_station(robot_p)) {
            follow_perimeter_wire(robot_p, &left_wheel_omega, &right_wheel_omega, signal);
        } else {
            robot_p->state.next = ROBOT_STATE_IN_BASE_STATION;
        }
    }

    motor_set_omega(&robot_p->left_motor, left_wheel_omega);
    motor_set_omega(&robot_p->right_motor, right_wheel_omega);

    return (0);
}

int robot_state_in_base_station(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_state_in_base_station, 1);

    /* Wait until plenty of energy is available. */
    if (battery_get_stored_energy_level(&robot_p->battery)
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
