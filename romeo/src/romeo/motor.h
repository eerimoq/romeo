/**
 * @file romeo/motor.h
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

#ifndef __ROMEO_MOTOR_H__
#define __ROMEO_MOTOR_H__

#include "simba.h"
#include "romeo.h"

/* Maximum motor rpm and angular velocity. */
#define MOTOR_RPM_MAX   30
#define MOTOR_OMEGA_MAX (((2 * PI) * MOTOR_RPM_MAX) / 60)

/* Motor rotation direction. */
#define MOTOR_DIRECTION_CW  0
#define MOTOR_DIRECTION_CCW 1

struct motor_t {
    float omega;
    struct pin_driver_t in1;
    struct pin_driver_t in2;
    struct pwm_driver_t enable;
    struct {
        struct adc_driver_t adc;
        struct {
            int samples[1];
        } ongoing;
        struct {
            int samples[1];
            int value;
        } updated;
    } current;
};

/**
 * Initialize motor object.
 * @param[out] motor_p Object to initialize.
 * @param[in] in1_p IN1 pin device.
 * @param[in] in2_p IN2 pin device.
 * @param[in] enable_p EN pwm device.
 * @param[in] current_adc_p ADC for current measurement.
 * @param[in] current_dev_p Analog pin to read current on.
 * @return zero(0) or negative error code
 */
int motor_init(struct motor_t *motor_p,
               struct pin_device_t *in1_p,
               struct pin_device_t *in2_p,
               struct pwm_device_t *enable_p,
               struct adc_device_t *current_adc_p,
               struct pin_device_t *current_dev_p);

/**
 * Set motor rotation direction.
 * @param[in] motor_p Initialized motor object.
 * @param[in] direction MOTOR_DIRECTION_CW or MOTOR_DIRECTION_CCW
 * @return zero(0) or negative error code
 */
int motor_set_direction(struct motor_t *motor_p,
                        int direction);

/**
 * Set motor rotation speed.
 * @param[in] motor_p Initialized motor object.
 * @param[in] omega Rotation speed in rad/s.
 * @return zero(0) or negative error code
 */
int motor_set_omega(struct motor_t *motor_p,
                    float omega);

/**
 * Start an asynchronous convertion of the motor current consumption. Call
 * motor_async_wait() to save the converted value in this object.
 * @param[in] motor_p Initialized motor object.
 * @return zero(0) or negative error code
 */
int motor_async_convert(struct motor_t *motor_p);

/**
 * Wait for the asynchronous convertion to finish.
 * @param[in] motor_p Initialized motor object.
 * @return zero(0) or negative error code
 */
int motor_async_wait(struct motor_t *motor_p);

/**
 * Update the object from the latest coverted samples.
 * @param[in] motor_p Initialized motor object.
 * @return zero(0) or negative error code.
 */
int motor_update(struct motor_t *motor_p);

/**
 * Get motor current consumption.
 * @param[in] motor_p Initialized motor object.
 * @return Motor current consumption.
 */
float motor_get_current(struct motor_t *motor_p);

#endif
