/**
 * @file robomower/controller.c
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

int controller_pid_init(struct controller_pid_t *pid_p,
                        float kp,
                        float ki,
                        float kd)
{
    pid_p->previous_error = 0.0f;
    pid_p->ackumulated_error = 0.0f;

    pid_p->kp = kp;
    pid_p->ki = ki;
    pid_p->kd = kd;

    return (0);
}

float controller_pid_calculate(struct controller_pid_t *pid_p,
                               float reference,
                               float actual)
{
    float error, p, i, d;

    error = (reference - actual);

    pid_p->ackumulated_error += error;

    p = pid_p->kp * error;
    i = pid_p->ki * pid_p->ackumulated_error;
    d = pid_p->kd * (error - pid_p->previous_error);

    pid_p->previous_error = error;

    return (p + i + d);
}
