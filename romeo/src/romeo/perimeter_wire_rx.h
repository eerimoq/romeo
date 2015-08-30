/**
 * @file romeo/perimeter_wire_rx.h
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

#ifndef __ROMEO_PERIMETER_WIRE_RX_H__
#define __ROMEO_PERIMETER_WIRE_RX_H__

#include "simba.h"

#define PERIMETER_WIRE_RX_SAMPLES_MAX (96)

struct perimeter_wire_rx_t {
    struct adc_driver_t adc;
    float filter_weight;
    struct {
        int samples[PERIMETER_WIRE_RX_SAMPLES_MAX];
    } ongoing;
    struct {
        int samples[PERIMETER_WIRE_RX_SAMPLES_MAX];
        float signal;
        float quality;
    } updated;
};

int perimeter_wire_rx_module_init(void);

/**
 * Initialize a perimeter wire instance from given data.
 * @param[out] perimeter_p Instance to be initialised.
 * @param[in] pin_dev_in1_p Pin device for in1.
 * @param[in] pin_dev_in2_p Pin device for in2.
 * @return zero(0) or negative error code.
 */
int perimeter_wire_rx_init(struct perimeter_wire_rx_t *perimeter_p,
                           struct adc_device_t *dev_p,
                           struct pin_device_t *pin_dev_p);

/**
 * Start an asynchronous convertion of the signal picked up by the inductor.
 * @param[in] perimeter_p Perimeter wire instance.
 * @return zero(0) or negative error code.
 */
int perimeter_wire_rx_async_convert(struct perimeter_wire_rx_t *perimeter_p);

/**
 * Wait for the asynchronous convertion to finish.
 * @param[in] perimeter_p Perimeter wire instance.
 * @return zero(0) or negative error code.
 */
int perimeter_wire_rx_async_wait(struct perimeter_wire_rx_t *perimeter_p);

/**
 * Update the object from the latest coverted samples.
 * @param[in] perimeter_p Perimeter wire instance.
 * @return zero(0) or negative error code.
 */
int perimeter_wire_rx_update(struct perimeter_wire_rx_t *perimeter_p);

/**
 * Get the signal on the wire.
 * @param[in] perimeter_p Perimeter wire instance.
 * @return The signal level.
 */
float perimeter_wire_rx_get_signal(struct perimeter_wire_rx_t *perimeter_p);

/**
 * Get the signal quality.
 * @param[in] perimeter_p Perimeter wire instance.
 * @return The signal quality.
 */
float perimeter_wire_rx_get_quality(struct perimeter_wire_rx_t *perimeter_p);

/**
 * Set the signal low pass filter weight.
 * @param[in] perimeter_p Perimeter wire instance.
 * @param[in] weight Weight to put on the average.
 * @return zero(0) or negative error code.
 */
int perimeter_wire_rx_set_filter_weight(struct perimeter_wire_rx_t *perimeter_p,
                                        float weight);

#endif
