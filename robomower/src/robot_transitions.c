/**
 * @file robot_transitions.c
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
#include "robot.h"

state_callback_t robot_transition__idle__starting(struct robot_t *robot_p)
{
    return (robot_state_starting);
}

state_callback_t robot_transition__starting__cutting(struct robot_t *robot_p)
{
    robot_p->substate.cutting.state = CUTTING_STATE_FORWARD;

    return (robot_state_cutting);
}

state_callback_t robot_transition__starting__in_base_station(struct robot_t *robot_p)
{
    return (robot_state_in_base_station);
}

state_callback_t robot_transition__cutting__idle(struct robot_t *robot_p)
{
    return (robot_state_idle);
}

state_callback_t robot_transition__cutting__searching_for_base_station(struct robot_t *robot_p)
{
    robot_p->substate.searching.state = SEARCHING_STATE_SEARCHING_FOR_PERIMETER_WIRE;

    return (robot_state_searching_for_base_station);
}

state_callback_t robot_transition__searching_for_base_station__in_base_station(struct robot_t *robot_p)
{
    return (robot_state_in_base_station);
}

state_callback_t robot_transition__searching_for_base_station__idle(struct robot_t *robot_p)
{
    return (robot_state_idle);
}

state_callback_t robot_transition__in_base_station__cutting(struct robot_t *robot_p)
{
    return (robot_state_cutting);
}

state_callback_t robot_transition__in_base_station__idle(struct robot_t *robot_p)
{
    return (robot_state_idle);
}
