/**
 * @file romeo/base_station.h
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

#ifndef __ROMEO_BASE_STATION_H__
#define __ROMEO_BASE_STATION_H__

#include "simba.h"
#include "romeo.h"

/* The processing loop period in milliseconds. */
#define BASE_STATION_PROCESS_PERIOD_MS 50L
#define BASE_STATION_PROCESS_PERIOD_NS (PROCESS_PERIOD_MS * 1000000L)

struct base_station_t {
    struct perimeter_wire_tx_t perimeter;
    struct adc_driver_t adc;
};

/**
 * Initialize base station object.
 * @param[out] base_station_p Base_Station object to initialize.
 * @return zero(0) or negative error code
 */
int base_station_init(struct base_station_t *base_station_p);

/**
 * Start of the base station. Do from idle to cutting state.
 * @param[in] base_station_p Initialized base_station object.
 * @return zero(0) or negative error code
 */
int base_station_start(struct base_station_t *base_station_p);

/**
 * Stop of the base station. Do from idle to cutting state.
 * @param[in] base_station_p Initialized base_station object.
 * @return zero(0) or negative error code
 */
int base_station_stop(struct base_station_t *base_station_p);

/**
 * Tick the base station.
 * @param[in] base_station_p Initialized base_station object.
 * @return zero(0) or negative error code
 */
int base_station_tick(struct base_station_t *base_station_p);

#endif
