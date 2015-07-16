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
#include "robomower.h"

/**
 * Calculate wheel turn rates from given robot speed and rotation.
 * @param[in] movement_p Movement instance.
 * @param[in] speed New speed in m/s. A negative value
 *                  means going in reverse.
 * @param[in] omega New turn rate in rad/s.
 * @param[out] left_wheel_omega Left wheel turn rate in rad/s.
 * @param[out] right_wheel_omega Right wheel turn rate in rad/s.
 * @return zero(0) or negative error code.
 */
int movement_calculate_wheels_omega(float speed,
                                    float omega,
                                    float *left_wheel_omega,
                                    float *right_wheel_omega);

#endif
