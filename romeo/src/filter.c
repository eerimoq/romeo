/**
 * @file romeo/filter.c
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

int filter_firf(const float *input_p,
                size_t number_of_samples,
                const float *coefficients,
                size_t number_of_coefficients,
                float *output_p)
{
    size_t i, n;
    float sum;
    size_t number_of_convolutions =
        (number_of_samples - number_of_coefficients + 1);

    for (n = 0; n < number_of_convolutions; n++) {
        sum = 0.0;
        for (i = 0; i < number_of_coefficients; i++) {
            sum += (coefficients[i] * input_p[n + i]);
        }
        output_p[n] = sum;
    }

    return (0);
}

int filter_fir(const int *input_p,
               size_t number_of_samples,
               const int *coefficients,
               size_t number_of_coefficients,
               int *output_p)
{
    size_t i, n;
    long sum;
    size_t number_of_convolutions =
        (number_of_samples - number_of_coefficients + 1);

    for (n = 0; n < number_of_convolutions; n++) {
        sum = 0;
        for (i = 0; i < number_of_coefficients; i++) {
            sum += (coefficients[i] * input_p[n + i]);
        }
        output_p[n] = sum;
    }

    return (0);
}
