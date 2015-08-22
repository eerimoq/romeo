/**
 * @file battery.c
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

/* Analog pin constants. */
#define ANALOG_VOLTAGE_MAX 5.0f
#define ANALOG_SAMPLES_MAX 1024

/* Measure over one of three resistors. */
#define VOLTAGE_DIVIDER_GAIN 3.0f

/* Battery constants. */
#define BATTERY_VOLTAGE_MAX (VOLTAGE_DIVIDER_GAIN * ANALOG_VOLTAGE_MAX)
#define BATTERY_VOLTAGE_PER_SAMPLE (BATTERY_VOLTAGE_MAX / ANALOG_SAMPLES_MAX)
#define BATTERY_VOLTAGE_EMPTY (11.5f)

FS_PARAMETER_DEFINE("/robot/battery/set_battery_voltage_full", battery_param_battery_voltage_full, int, 13);

int battery_init(struct battery_t *battery_p,
               struct adc_device_t *dev_p,
               struct pin_device_t *pin_dev_p)
{
    battery_p->battery_voltage_full = FS_PARAMETER(battery_param_battery_voltage_full);

    adc_init(&battery_p->adc,
             dev_p,
             pin_dev_p,
             ADC_REFERENCE_VCC,
             1000);

    return (0);
}

int battery_async_convert(struct battery_t *battery_p)
{
    /* Start asynchronous convertion. */
    return (adc_async_convert(&battery_p->adc,
                              battery_p->ongoing.samples,
                              membersof(battery_p->ongoing.samples)));
}

int battery_async_wait(struct battery_t *battery_p)
{
    /* Wait for ongoing asynchronous convertion to finish. */
    if (!adc_async_wait(&battery_p->adc)) {
        std_printk(STD_LOG_WARNING, FSTR("battery convertion has not finished"));
    }

    return (0);
}

int battery_update(struct battery_t *battery_p)
{
    int sample, stored_energy_level;
    float battery_voltage;

    /* Save latest sample. */
    memcpy(battery_p->updated.samples,
           battery_p->ongoing.samples,
           sizeof(battery_p->updated.samples));

    sample = battery_p->updated.samples[0];

    /* Remove when measured after charging. */
    battery_p->battery_voltage_full = FS_PARAMETER(battery_param_battery_voltage_full);

    if (battery_p->battery_voltage_full < BATTERY_VOLTAGE_EMPTY) {
        battery_p->battery_voltage_full = BATTERY_VOLTAGE_EMPTY;
    }

    /* Calculate the battery voltage. */
    battery_voltage = (sample * BATTERY_VOLTAGE_PER_SAMPLE);

    /* Use the battery voltage to calculate the stored energy level. */
    if (battery_voltage < BATTERY_VOLTAGE_EMPTY) {
        stored_energy_level = 0;
    } else if (battery_voltage > battery_p->battery_voltage_full) {
        stored_energy_level = 100;
    } else {
        stored_energy_level = ((100.0f * (battery_voltage - BATTERY_VOLTAGE_EMPTY))
                               / (battery_p->battery_voltage_full - BATTERY_VOLTAGE_EMPTY));
    }

    battery_p->updated.battery_voltage = battery_voltage;
    battery_p->updated.stored_energy_level = stored_energy_level;

    return (0);
}

int battery_get_stored_energy_level(struct battery_t *battery_p)
{
    return (battery_p->updated.stored_energy_level);
}

float battery_get_battery_voltage(struct battery_t *battery_p)
{
    return (battery_p->updated.battery_voltage);
}
