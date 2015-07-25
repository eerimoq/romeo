/**
 * @file romeo/filter.h
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

#ifndef __ROMEO_FILTER_H__
#define __ROMEO_FILTER_H__

#include "simba.h"

/**
 * Discrete time Finite Impule Response (FIR) filter working on float.
 *
 * An FIR filter is designed by finding the coefficients and filter
 * order that meet certain specifications, which can be in the
 * time-domain (e.g. a matched filter) and/or the frequency domain
 * (most common). Matched filters perform a cross-correlation between
 * the input signal and a known pulse-shape. The FIR convolution is a
 * cross-correlation between the input signal and a time-reversed copy
 * of the impulse-response. Therefore, the matched-filter's impulse
 * response is "designed" by sampling the known pulse-shape and using
 * those samples in reverse order as the coefficients of the filter.
 *
 * @param[in] input_p Input samples, often known as x.
 * @param[in] number_of_samples Number of samples in input_p.
 * @param[in] coefficients Filter coefficients.
 * @param[in] number_of_coefficients Number of filter coefficients.
 * @param[out] output_p Output samples, often known as y.
 * @return zero(0) or negative error code.
 */
int filter_firf(const float *input_p,
                size_t number_of_samples,
                const float *coefficients,
                size_t number_of_coefficients,
                float *output_p);

/**
 * Discrete time Finite Impule Response (FIR) filter working on
 * integers.
 *
 * @param[in] input_p Input samples, often known as x.
 * @param[in] number_of_samples Number of samples in input_p.
 * @param[in] coefficients Filter coefficients.
 * @param[in] number_of_coefficients Number of filter coefficients.
 * @param[out] output_p Output samples, often known as y.
 * @return zero(0) or negative error code.
 */
int filter_fir(const int *input_p,
                size_t number_of_samples,
                const int *coefficients,
                size_t number_of_coefficients,
                int *output_p);

#endif
