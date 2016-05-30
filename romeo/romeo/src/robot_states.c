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

#define FOLLOW_KP 1.0f
#define FOLLOW_KI 0.0f
#define FOLLOW_KD 0.5f

/* Counters. */
static struct fs_counter_t counter_state_idle;
static struct fs_counter_t counter_state_starting;
static struct fs_counter_t counter_state_cutting;
static struct fs_counter_t counter_state_searching_for_base_station;
static struct fs_counter_t counter_state_in_base_station;

static struct fs_counter_t counter_cutting_state_forward;
static struct fs_counter_t counter_cutting_state_backwards;
static struct fs_counter_t counter_cutting_state_rotating;

static struct fs_counter_t counter_odometer;

static struct fs_counter_t counter_is_stuck;

static struct fs_counter_t counter_is_inside_perimeter_wire;
static struct fs_counter_t counter_is_outside_perimeter_wire;

/* Parameters. */
int parameter_charging_value = 0;
static struct fs_parameter_t parameter_charging;
static int parameter_search_for_the_base_station_value = -1;
static struct fs_parameter_t parameter_search_for_the_base_station;

/**
 * @return true(1) if the robot should start searching for the base
 *         station, otherwise false(0).
 */
static int is_time_to_search_for_base_station(struct robot_t *robot_p)
{
    if (parameter_search_for_the_base_station_value != -1) {
        return (parameter_search_for_the_base_station_value != 0);
    }

    return (battery_get_stored_energy_level(&robot_p->battery) <= 20);
}

static int is_inside_perimeter_wire(float signal)
{
    int is_inside = (signal >= 3.0f);

    if (is_inside == 1) {
        fs_counter_increment(&counter_is_inside_perimeter_wire, 1);
    } else {
        fs_counter_increment(&counter_is_outside_perimeter_wire, 1);
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
        fs_counter_increment(&counter_is_stuck, 1);
        return (1);
    }

    return (0);
}

static int is_charging(struct robot_t *robot_p)
{
    return (parameter_charging_value);
}

static int is_arriving_to_base_station(struct robot_t *robot_p)
{
    return (is_stuck(robot_p) && is_charging(robot_p));
}

/**
 * Follow the wire counter clockwise. The signal on the left hand side
 * of the robot should be positive, while the signal on the right hand
 * side should be negative.
 */
static int follow_perimeter_wire(struct robot_t *robot_p,
                                 struct searching_for_base_station_state_t *searching_p,
                                 float *speed_p,
                                 float *omega_p,
                                 float signal)
{
    float control;

    /* Just wait if the robot is stuck. The default speed and omeg are
       zero. */
    if (is_stuck(robot_p)) {
        return (0);
    }

    /* Try to stay on top of the wire, hence set the actual value to
       0.0f. */
    control = controller_pid_calculate(&searching_p->pid_controller,
                                       0.0f,
                                       signal);

    /* For debug. */
    searching_p->control = control;

    *speed_p = 0.07f;

    if (fabs(control) < 11.0f) {
        *omega_p = (0.05f * (control / 10.0f));
    } else {
        *omega_p = (0.6f * (control / 9.0f));
    }

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
        log_object_print(NULL, LOG_NOTICE, FSTR("cutting stuck"));

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
            fs_counter_increment(&counter_cutting_state_forward, 1);

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
            fs_counter_increment(&counter_cutting_state_backwards, 1);

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
            fs_counter_increment(&counter_cutting_state_rotating, 1);

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

int robot_states_module_init(void)
{
    /* Counters. */
    fs_counter_init(&counter_state_idle,
                    FSTR("/robot/counters/state_idle"),
                    0);
    fs_counter_register(&counter_state_idle);
    
    fs_counter_init(&counter_state_starting,
                    FSTR("/robot/counters/state_starting"),
                    0);
    fs_counter_register(&counter_state_starting);

    fs_counter_init(&counter_state_cutting,
                    FSTR("/robot/counters/state_cutting"),
                    0);
    fs_counter_register(&counter_state_cutting);

    fs_counter_init(&counter_state_searching_for_base_station,
                    FSTR("/robot/counters/state_searching_for_base_station"),
                    0);
    fs_counter_register(&counter_state_searching_for_base_station);

    fs_counter_init(&counter_state_in_base_station,
                    FSTR("/robot/counters/state_in_base_station"),
                    0);
    fs_counter_register(&counter_state_in_base_station);

    fs_counter_init(&counter_cutting_state_forward,
                    FSTR("/robot/counters/cutting_state_forward"),
                    0);
    fs_counter_register(&counter_cutting_state_forward);

    fs_counter_init(&counter_cutting_state_backwards,
                    FSTR("/robot/counters/cutting_state_backwards"),
                    0);
    fs_counter_register(&counter_cutting_state_backwards);

    fs_counter_init(&counter_cutting_state_rotating,
                    FSTR("/robot/counters/cutting_state_rotating"),
                    0);
    fs_counter_register(&counter_cutting_state_rotating);

    fs_counter_init(&counter_odometer,
                    FSTR("/robot/counters/odometer"),
                    0);
    fs_counter_register(&counter_odometer);

    fs_counter_init(&counter_is_stuck,
                    FSTR("/robot/counters/is_stuck"),
                    0);
    fs_counter_register(&counter_is_stuck);

    fs_counter_init(&counter_is_inside_perimeter_wire,
                    FSTR("/robot/counters/is_inside_perimeter_wire"),
                    0);
    fs_counter_register(&counter_is_inside_perimeter_wire);

    fs_counter_init(&counter_is_outside_perimeter_wire,
                    FSTR("/robot/counters/is_outside_perimeter_wire"),
                    0);
    fs_counter_register(&counter_is_outside_perimeter_wire);

    /* Parameters. */
    fs_parameter_init(&parameter_charging,
                      FSTR("/robot/parameters/charging"),
                      fs_cmd_parameter_int,
                      &parameter_charging_value);
    fs_parameter_register(&parameter_charging);

    fs_parameter_init(&parameter_search_for_the_base_station,
                      FSTR("/robot/parameters/search_for_the_base_station"),
                      fs_cmd_parameter_int,
                      &parameter_search_for_the_base_station_value);
    fs_parameter_register(&parameter_search_for_the_base_station);

    return (0);
}

int robot_state_idle(struct robot_t *robot_p)
{
    float left_wheel_omega;
    float right_wheel_omega;

    fs_counter_increment(&counter_state_idle, 1);

    /* Robot standing still in idle state. */
    left_wheel_omega = 0.0f;
    right_wheel_omega = 0.0f;

    motor_set_omega(&robot_p->left_motor, left_wheel_omega);
    motor_set_omega(&robot_p->right_motor, right_wheel_omega);

    return (0);
}

int robot_state_starting(struct robot_t *robot_p)
{
    fs_counter_increment(&counter_state_starting, 1);

    robot_p->state.next = ROBOT_STATE_CUTTING;

    return (0);
}

int robot_state_cutting(struct robot_t *robot_p)
{
    float left_wheel_omega;
    float right_wheel_omega;
    float speed;
    float omega;

    fs_counter_increment(&counter_state_cutting, 1);

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
        fs_counter_increment(&counter_odometer, speed * PROCESS_PERIOD_MS);
    }

    return (0);
}

int robot_state_searching_for_base_station(struct robot_t *robot_p)
{
    float signal;
    float speed;
    float omega;
    float left_wheel_omega;
    float right_wheel_omega;
    struct searching_for_base_station_state_t *searching_p =
        &robot_p->substate.searching;

    speed = 0.0f;
    omega = 0.0f;

    signal = perimeter_wire_rx_get_signal(&robot_p->perimeter);

    fs_counter_increment(&counter_state_searching_for_base_station, 1);

    if (searching_p->state == SEARCHING_STATE_SEARCHING_FOR_PERIMETER_WIRE) {
        /* No movement if the robot is stuck. */
        if (!is_stuck(robot_p)) {
            /* Find the perimeter wire. */
            if (is_inside_perimeter_wire(signal)) {
                speed = 0.1f;
                omega = 0.0f;
            } else {
                /* Perimeter wire found. */
                searching_p->state = SEARCHING_STATE_ALIGNING_WITH_WIRE;
            }
        } else {
            /* TODO: try to get free */
        }
    } else if (searching_p->state == SEARCHING_STATE_ALIGNING_WITH_WIRE) {
        /* No movement if the robot is stuck. */
        if (!is_stuck(robot_p)) {
            /* Rotate until the robot is within the perimeter wire again. */
            if (!is_inside_perimeter_wire(signal)) {
                omega = 0.4f;
            } else {
                /* Perimeter wire found. */
                searching_p->state = SEARCHING_STATE_FOLLOWING_PERIMETER_WIRE;

                /* High sensitivity on sensors and actuators when following the wire. */
                perimeter_wire_rx_set_filter_weight(&robot_p->perimeter, 2.0f);
                motor_set_filter_weight(&robot_p->left_motor, 1.0f);
                motor_set_filter_weight(&robot_p->right_motor, 1.0f);

                /* Initialize the PID controller. */
                controller_pid_init(&searching_p->pid_controller,
                                    FOLLOW_KP,
                                    FOLLOW_KI,
                                    FOLLOW_KD);
                searching_p->tick.is_inside = 0;
                searching_p->tick.count = 0;
            }
        } else {
            /* TODO: try to get free */
        }
    } else if (searching_p->state == SEARCHING_STATE_FOLLOWING_PERIMETER_WIRE) {
        /* Follow the perimeter wire to the base station. */
        if (!is_arriving_to_base_station(robot_p)) {
            follow_perimeter_wire(robot_p,
                                  searching_p,
                                  &speed,
                                  &omega,
                                  signal);
        } else {
            robot_p->state.next = ROBOT_STATE_IN_BASE_STATION;
        }
    } else {
        /* Should not happen. */
        robot_p->state.next = ROBOT_STATE_IDLE;
    }

    /* Convert the robot speeds to wheel motor angular velocities. */
    movement_calculate_wheels_omega(&robot_p->movement,
                                    speed,
                                    omega,
                                    &left_wheel_omega,
                                    &right_wheel_omega);

    motor_set_omega(&robot_p->left_motor, left_wheel_omega);
    motor_set_omega(&robot_p->right_motor, right_wheel_omega);

    return (0);
}

int robot_state_in_base_station(struct robot_t *robot_p)
{
    fs_counter_increment(&counter_state_in_base_station, 1);

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
