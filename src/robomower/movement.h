/**
 * @file robomower/movement.h
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

#ifndef __ROBOMOWER_MOVEMENT_H__
#define __ROBOMOWER_MOVEMENT_H__

#include "simba.h"

struct movement_motor_t {
    struct pin_driver_t in1;
    struct pin_driver_t in2;
    struct pin_driver_t enable;
};

struct movement_t {
    int speed;
    int omega;
    struct movement_motor_t left_motor;
    struct movement_motor_t right_motor;
};

/**
 * Initialize a movement instance from given data.
 * @param[out] movement_p Instance to be initialised.
 * @param[in] pin_dev_in1_p Pin device for in1.
 * @param[in] pin_dev_in2_p Pin device for in2.
 * @return zero(0) or negative error code.
 */
int movement_init(struct movement_t *movement_p,
                  struct pin_device_t *left_motor_in1_p,
                  struct pin_device_t *left_motor_in2_p,
                  struct pin_device_t *left_motor_enable_p,
                  struct pin_device_t *right_motor_in1_p,
                  struct pin_device_t *right_motor_in2_p,
                  struct pin_device_t *right_motor_enable_p);

/**
 * Set movement speed and turn rate.
 * @param[in] movement_p Movement instance.
 * @param[in] speed New speed in cm/s. A negative value
 *                  means going in reverse.
 * @param[in] omega New turn rate in rad/100*s.
 * @return zero(0) or negative error code.
 */
int movement_set(struct movement_t *movement_p,
                 int speed,
                 int omega);

#endif
