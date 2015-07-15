/**
 * @file robomower/movement.c
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

/* Geometry. */
#define WHEEL_DISTANCE_CM      30
#define WHEEL_CIRCUMFERENCE_CM 40

static int set_motor_speed(struct movement_motor_t *motor_p,
                           int motor_speed)
{
    /* Rotation direction pins. */
    if (motor_speed > 0) {
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

int movement_init(struct movement_t *movement_p,
                  struct pin_device_t *left_motor_in1_p,
                  struct pin_device_t *left_motor_in2_p,
                  struct pin_device_t *left_motor_enable_p,
                  struct pin_device_t *right_motor_in1_p,
                  struct pin_device_t *right_motor_in2_p,
                  struct pin_device_t *right_motor_enable_p)
{
    struct movement_motor_t *left_p;
    struct movement_motor_t *right_p;

    left_p = &movement_p->left_motor;
    right_p = &movement_p->right_motor;

    /* Initialize all pins connected to the motor controllers. */
    pin_init(&left_p->in1, left_motor_in1_p, PIN_OUTPUT);
    pin_init(&left_p->in2, left_motor_in2_p, PIN_OUTPUT);
    pin_init(&left_p->enable, left_motor_enable_p, PIN_OUTPUT);
    pin_init(&right_p->in1, right_motor_in1_p, PIN_OUTPUT);
    pin_init(&right_p->in2, right_motor_in2_p, PIN_OUTPUT);
    pin_init(&right_p->enable, right_motor_enable_p, PIN_OUTPUT);

    return (0);
}

int movement_set(struct movement_t *movement_p,
                 int speed,
                 int omega)
{
    int left_motor_speed;
    int right_motor_speed;
    int omega_wheel_speed;

    std_printk(STD_LOG_DEBUG,
               FSTR("movement speed %d cm/s and omega %d rad/100*s"),
               speed, omega);

    /* The base speed of the wheels. */
    left_motor_speed = (100 * speed);
    right_motor_speed = (100 * speed);

    /* Omega gives the difference in speed for the two motors. */
    omega_wheel_speed = ((omega * WHEEL_DISTANCE_CM / 2) / 2);

    /* Add/subtract the speed component from omega. */
    if (omega > 0) {
        left_motor_speed += omega_wheel_speed;
        right_motor_speed -= omega_wheel_speed;
    } else {
        left_motor_speed -= omega_wheel_speed;
        right_motor_speed += omega_wheel_speed;
    }

    left_motor_speed /= 100;
    right_motor_speed /= 100;

    std_printk(STD_LOG_DEBUG,
               FSTR("speeds: left %d, right %d [cm/s]"),
               left_motor_speed, right_motor_speed);

    movement_p->speed = speed;
    movement_p->omega = omega;

    /* Set the motor speeds. */
    set_motor_speed(&movement_p->left_motor, left_motor_speed);
    set_motor_speed(&movement_p->right_motor, right_motor_speed);

    return (0);
}
