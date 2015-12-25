/**
 * @file romeo/movement.h
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

#ifndef __ROMEO_MOVEMENT_H__
#define __ROMEO_MOVEMENT_H__

#include "simba.h"
#include "romeo.h"

struct movement_t {
    float wheel_distance_in_meters;
    float wheel_radius_in_meters;
};

/**
 * Initialize movement object with given geometry.
 * @param[out] movement_p Obkect to initialize.
 * @param[in] wheel_distance_in_meters
 * @param[out] wheel_radius_in_meters.
 * @return zero(0) or negative error code.
 */
int movement_init(struct movement_t *movement_p,
                  float wheel_distance_in_meters,
                  float wheel_radius_in_meters);

/**
 * Calculate wheel turn rates from given robot speed and rotation.
 * @param[in] movement_p Movement instance.
 * @param[in] speed New speed in m/s. A negative value
 *                  means going in reverse.
 * @param[in] omega New turn rate in rad/s.
 * @param[out] left_wheel_omega_p Left wheel turn rate in rad/s.
 * @param[out] right_wheel_omega_p Right wheel turn rate in rad/s.
 * @return zero(0) if the left and right wheel rotations speeds
 *         are successfully calculated and within allowed ranges,
 *         otherwise negative error code
 */
int movement_calculate_wheels_omega(struct movement_t *movement_p,
                                    float speed,
                                    float omega,
                                    float *left_wheel_omega_p,
                                    float *right_wheel_omega_p);

/**
 * Get the maximum speed.
 * @param[in] movement_p Movement instance.
 * @return the maximum speed
 */
float movement_get_maximum_speed(struct movement_t *movement_p);

/**
 * Get the maximum omega.
 * @param[in] movement_p Movement instance.
 * @return the maximum omega
 */
float movement_get_maximum_omega(struct movement_t *movement_p);

#endif
