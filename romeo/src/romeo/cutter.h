/**
 * @file romeo/cutter.h
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

#ifndef __ROMEO_CUTTER_H__
#define __ROMEO_CUTTER_H__

#include "simba.h"

struct cutter_t {
    uint8_t state;
    uint8_t pos;
    struct pin_driver_t pin_in1;
    struct pin_driver_t pin_in2;
};

/**
 * Initialize a cutter instance from given data.
 * @param[out] cutter_p Instance to be initialised.
 * @param[in] pin_dev_in1_p Pin device for in1.
 * @param[in] pin_dev_in2_p Pin device for in2.
 * @return zero(0) or negative error code.
 */
int cutter_init(struct cutter_t *cutter_p,
                struct pin_device_t *pin_dev_in1_p,
                struct pin_device_t *pin_dev_in2_p,
                struct pin_device_t *pin_dev_enable_p);

/**
 * Start the cutter.
 * @param[in] cutter_p Cutter instance.
 * @return zero(0) or negative error code.
 */
int cutter_start(struct cutter_t *cutter_p);

/**
 * Stop the cutter.
 * @param[in] cutter_p Cutter instance.
 * @return zero(0) or negative error code.
 */
int cutter_stop(struct cutter_t *cutter_p);

#endif
