/**
 * @file testdata.h
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

struct testdata_t {
    int8_t energy_level;
    float perimeter_signal;
    float left_wheel_omega;
    float right_wheel_omega;
};

/* Used by the stubs.*/
extern int testdata_index;
extern const struct testdata_t FAR *testdata_p;

/* Used by the testcase to prepare the stubs. */
extern FAR const struct testdata_t test_automatic_testdata[];
