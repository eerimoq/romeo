/**
 * @file motor.c
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

#include "simba.h"
#include "romeo.h"

int motor_init(struct motor_t *motor_p,
               struct pin_device_t *in1_p,
               struct pin_device_t *in2_p,
               struct pwm_device_t *enable_p,
               struct adc_device_t *current_adc_p,
               struct pin_device_t *current_dev_p)
{
    motor_p->omega = 0.0f;

    /* Initialize all pins connected to the motor controllers. */
    pin_init(&motor_p->in1, in1_p, PIN_OUTPUT);
    pin_init(&motor_p->in2, in2_p, PIN_OUTPUT);

    pwm_init(&motor_p->enable, enable_p);

    adc_init(&motor_p->current.adc,
             current_adc_p,
             current_dev_p,
             ADC_REFERENCE_VCC,
             1000);

    return (0);
}

int motor_start(struct motor_t *motor_p)
{
    /* Start first asynchronous convertion. */
    adc_async_convert(&motor_p->current.adc,
                      &motor_p->current.sample,
                      1);

    return (0);
}

/**
 * Set motor direction.
 * @param[in] direction 0 for CW, 1 for CCW
 * @return zero(0) or negative error code
 */
int motor_set_direction(struct motor_t *motor_p,
                        int direction)
{
    pin_write(&motor_p->in1, direction);
    pin_write(&motor_p->in2, !direction);

    return (0);
}

int motor_set_omega(struct motor_t *motor_p,
                    float omega)
{
    unsigned int duty;
    int direction;

    /* Motor rotation direction. */
    if (omega > 0.0f) {
        direction = MOTOR_DIRECTION_CW;
    } else {
        direction = MOTOR_DIRECTION_CCW;
        /* Positive omega for pwm duty calculation. */
        omega *= -1.0f;
    }

    motor_set_direction(motor_p, direction);

    /* Low pass filtering of the angular velocity. */
    omega = (2.0f * motor_p->omega + 1.0f * omega) / 3.0f;

    motor_p->omega = omega;

    /* Control the motor speed using PWM signal. */
    duty = (255.0f * omega) / MOTOR_OMEGA_MAX;
    pwm_set_duty(&motor_p->enable, duty);

    std_printk(STD_LOG_DEBUG,
               FSTR("motor: direction = %d, duty = %u"),
               direction,
               duty);

    return (0);
}

float motor_get_current(struct motor_t *motor_p)
{
    int sample;

    /* Wait for ongoing asynchronous convertion to finish. */
    if (!adc_async_wait(&motor_p->current.adc)) {
        std_printk(STD_LOG_WARNING, FSTR("motor convertion has not finished"));
    }

    /* Copy sample to local variable. */
    sample = motor_p->current.sample;

    /* Start next asynchronous convertion. */
    adc_async_convert(&motor_p->current.adc,
                      &motor_p->current.sample,
                      1);

    return (sample);
}