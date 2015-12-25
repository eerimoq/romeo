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
 * This file is part of the Romeo project.
 */

#ifndef __ROBOT_H__
#define __ROBOT_H__

#include "simba.h"
#include "romeo.h"

int robot_state_idle(struct robot_t *robot_p);
int robot_state_starting(struct robot_t *robot_p);
int robot_state_cutting(struct robot_t *robot_p);
int robot_state_searching_for_base_station(struct robot_t *robot_p);
int robot_state_in_base_station(struct robot_t *robot_p);

state_callback_t robot_transition__idle__starting(struct robot_t *robot_p);
state_callback_t robot_transition__starting__cutting(struct robot_t *robot_p);
state_callback_t robot_transition__starting__in_base_station(struct robot_t *robot_p);
state_callback_t robot_transition__cutting__idle(struct robot_t *robot_p);
state_callback_t robot_transition__cutting__searching_for_base_station(struct robot_t *robot_p);
state_callback_t robot_transition__searching_for_base_station__in_base_station(struct robot_t *robot_p);
state_callback_t robot_transition__searching_for_base_station__idle(struct robot_t *robot_p);
state_callback_t robot_transition__in_base_station__cutting(struct robot_t *robot_p);
state_callback_t robot_transition__in_base_station__idle(struct robot_t *robot_p);

#endif
