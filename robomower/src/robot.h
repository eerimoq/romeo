/**
 * @file robot.h
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

#ifndef __ROBOT_H__
#define __ROBOT_H__

#include "simba.h"
#include "robomower.h"

/* The processing loop period in milliseconds. */
#define PROCESS_PERIOD_MS 50L
#define PROCESS_PERIOD_NS (PROCESS_PERIOD_MS * 1000000L)

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

/* Searching for base station states. */
#define SEARCHING_STATE_SEARCHING_FOR_PERIMETER_WIRE 0
#define SEARCHING_STATE_FOLLOWING_PERIMETER_WIRE     1

/* Drive backwards a number of ticks. */
#ifndef CUTTING_STATE_BACKWARDS_TICKS
#    define CUTTING_STATE_BACKWARDS_TICKS 64
#endif

/* Rotate a random number of ticks. */
#ifndef CUTTING_STATE_ROTATING_TICKS
#    include <math.h>
#    define CUTTING_STATE_ROTATING_TICKS  (31 + (rand() & 0x3f))
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
};

struct robot_t {
    volatile int mode;
    struct robot_state_t state;
    union {
        struct cutting_state_t cutting;
        struct searching_for_base_station_state_t searching;
    } substate;
    volatile struct {
        float speed;
        float omega;
    } manual;
    struct thrd_t *self_p;
    struct timer_t ticker;
    struct motor_t left_motor;
    struct motor_t right_motor;
    struct perimeter_wire_rx_t perimeter;
    struct controller_pid_t pid;
    struct power_t power;
    struct {
        int processing_time;
    } debug;
};

int state_idle(struct robot_t *robot_p);
int state_starting(struct robot_t *robot_p);
int state_cutting(struct robot_t *robot_p);
int state_searching_for_base_station(struct robot_t *robot_p);
int state_in_base_station(struct robot_t *robot_p);

state_callback_t transition__idle__starting(struct robot_t *robot_p);
state_callback_t transition__starting__cutting(struct robot_t *robot_p);
state_callback_t transition__starting__in_base_station(struct robot_t *robot_p);
state_callback_t transition__cutting__idle(struct robot_t *robot_p);
state_callback_t transition__cutting__searching_for_base_station(struct robot_t *robot_p);
state_callback_t transition__searching_for_base_station__in_base_station(struct robot_t *robot_p);
state_callback_t transition__searching_for_base_station__idle(struct robot_t *robot_p);
state_callback_t transition__in_base_station__cutting(struct robot_t *robot_p);
state_callback_t transition__in_base_station__idle(struct robot_t *robot_p);

#endif
