/**
 * @file robomower/robot.h
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

#ifndef __ROBOMOWER_ROBOT_H__
#define __ROBOMOWER_ROBOT_H__

#include "simba.h"
#include "robomower.h"

/**
 * Robot main loop.
 * @return zero(0) or negative error code
 */
int robot_entry();

#endif
