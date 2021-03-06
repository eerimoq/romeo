/**
 * @file romeo/robot.h
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

#ifndef __ROMEO_ROBOT_H__
#define __ROMEO_ROBOT_H__

#include "simba.h"
#include "romeo.h"

/* The processing loop period in milliseconds. */
#define PROCESS_PERIOD_MS 30L
#define PROCESS_PERIOD_NS (PROCESS_PERIOD_MS * 1000000L)

/* Robot modes. */
enum robot_mode_t {
    ROBOT_MODE_MANUAL = 0,
    ROBOT_MODE_AUTOMATIC
};

/* Robot states. */
enum e_robot_state_t {
    ROBOT_STATE_IDLE = 0,
    ROBOT_STATE_STARTING,
    ROBOT_STATE_CUTTING,
    ROBOT_STATE_SEARCHING_FOR_BASE_STATION,
    ROBOT_STATE_IN_BASE_STATION
};

/* Cutting states. */
enum e_cutting_state_t {
    CUTTING_STATE_FORWARD = 0,
    CUTTING_STATE_BACKWARDS,
    CUTTING_STATE_ROTATING
};

/* Searching for base station states. */
enum e_searching_state_t {
    SEARCHING_STATE_SEARCHING_FOR_PERIMETER_WIRE = 0,
    SEARCHING_STATE_ALIGNING_WITH_WIRE,
    SEARCHING_STATE_FOLLOWING_PERIMETER_WIRE
};

/* Drive backwards a number of ticks. */
#ifndef CUTTING_STATE_BACKWARDS_TICKS
#    define CUTTING_STATE_BACKWARDS_TICKS (1500L / PROCESS_PERIOD_MS)
#endif

#ifndef WATCHDOG_TIMEOUT_TICKS
#    define WATCHDOG_TIMEOUT_TICKS (1500L / PROCESS_PERIOD_MS)
#endif

/* Rotate a random number of ticks. */
#ifndef CUTTING_STATE_ROTATING_TICKS
#    include <math.h>
#    define CUTTING_STATE_ROTATING_TICKS                \
    ((500L + (rand() % 512L)) / PROCESS_PERIOD_MS)
#endif

struct robot_t;

typedef int (*state_callback_t)(struct robot_t *);
typedef state_callback_t (*transition_callback_t)(struct robot_t *);

struct robot_state_t {
    volatile int current;
    volatile int next;
    int (*callback)(struct robot_t *);
};

struct cutting_state_t {
    int state;
    int ticks_left;
};

struct searching_for_base_station_state_t {
    int state;
    struct controller_pid_t pid_controller;
    float control;
    struct {
        int is_inside;
        int count;
    } tick;
};

struct robot_t {
    volatile int mode;
    struct robot_state_t state;
    union {
        struct cutting_state_t cutting;
        struct searching_for_base_station_state_t searching;
    } substate;
    struct movement_t movement;
    struct motor_t left_motor;
    struct motor_t right_motor;
    struct perimeter_wire_rx_t perimeter;
    struct battery_t battery;
    struct watchdog_t watchdog;
    struct {
        int tick_time;
    } debug;
    volatile struct {
        float speed;
        float omega;
    } manual;
};

/**
 * Initialize the robot module.
 * @return zero(0) or negative error code
 */
int robot_module_init(void);

/**
 * Initialize robot object.
 * @param[out] robot_p Robot object to initialize.
 * @return zero(0) or negative error code
 */
int robot_init(struct robot_t *robot_p);

/**
 * Start of the robot. Do from idle to cutting state.
 * @param[in] robot_p Initialized robot object.
 * @return zero(0) or negative error code
 */
int robot_start(struct robot_t *robot_p);

/**
 * Stop of the robot. Do from idle to cutting state.
 * @param[in] robot_p Initialized robot object.
 * @return zero(0) or negative error code
 */
int robot_stop(struct robot_t *robot_p);

/**
 * Tick the robot.
 * @param[in] robot_p Initialized robot object.
 * @return zero(0) or negative error code
 */
int robot_tick(struct robot_t *robot_p);

/**
 * Kick the robot watchdog.
 * @param[in] robot_p Initialized robot object.
 * @return zero(0) or negative error code
 */
int robot_watchdog_kick(struct robot_t *robot_p);

#endif
