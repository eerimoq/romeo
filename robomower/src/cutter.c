/**
 * @file robomower/cutter.c
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

int cutter_init(struct cutter_t *cutter_p,
                struct pin_device_t *pin_dev_in1_p,
                struct pin_device_t *pin_dev_in2_p,
                struct pin_device_t *pin_dev_enable_p)
{
    return (0);
}

int cutter_start(struct cutter_t *cutter_p)
{
    return (0);
}

int cutter_stop(struct cutter_t *cutter_p)
{
    return (0);
}
