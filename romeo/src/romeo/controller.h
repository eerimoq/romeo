/**
 * @file romeo/controller.h
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

#ifndef __ROMEO_CONTROLLER_H__
#define __ROMEO_CONTROLLER_H__

#include "simba.h"
#include "romeo.h"

struct controller_pid_t {
    /* State variables. */
    float previous_error;
    float ackumulated_error;
    /* Regulator coefficients. */
    float kp;
    float ki;
    float kd;
};

/**
 * Initialize proportional-integral-derivate controller.
 * @param[out] pid_p PID controller object to initialize.
 * @param[in] kp Proportional gain.
 * @param[in] ki Integral gain.
 * @param[in] kd Derivate gain.
 * @return zero(0) or negative error code.
 */
int controller_pid_init(struct controller_pid_t *pid_p,
                        float kp,
                        float ki,
                        float kd);

/**
 * Calculate control signal from given reference and actual signals.
 * @param[in] pid_p Initialized PID controller instance.
 * @param[in] reference Reference signal, often known as 'r'.
 * @param[in] actual Actual signal, often known as 'y'.
 * @return Control signal, often known as 'u'.
 */
float controller_pid_calculate(struct controller_pid_t *pid_p,
                               float reference,
                               float actual);

#endif
