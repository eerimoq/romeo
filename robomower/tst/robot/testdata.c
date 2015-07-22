/**
 * @file testdata.c
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
#include "testdata.h"

int testdata_index;
const struct testdata_t FAR *testdata_p;

/* Input signal to robot. */
FAR const struct testdata_t test_automatic_testdata[] = {
    /* 0 */
    /* cutting forward */
    {  100,  24.0f,  1.570795f,  1.570795f,    0.0f },
    {   90,  24.0f,  1.570795f,  1.570795f,    0.0f },
    {   80,  24.0f,  1.570795f,  1.570795f,    0.0f },
    /* perimeter found. stop.*/
    {   70, -24.0f,  0.0f,       0.0f,         0.0f },
    /* cutting backwards.*/
    {   60, -15.0f, -1.570795f, -1.570795f,    0.0f },
                   
    /* 5 */
    /* stop.*/
    {   50,  12.0f,  0.0f,       0.0f,         0.0f },
    /* cutting rotate.*/
    {   40,  24.0f,  0.235619f, -0.235619f,    0.0f },
    {   30,  24.0f,  0.0f,       0.0f,         0.0f },
    /* cutting forward.*/
    {   30,  24.0f,  1.570795f,  1.570795f,    0.0f },
    {   30,  24.0f,  1.570795f,  1.570795f,    0.0f },
    {   30,  24.0f,  1.570795f,  1.570795f,    0.0f },
    /* stuck, don't move. */
    {   -2,  24.0f,  0.0f,       0.0f,       700.0f },
                   
    /* 10 */
    /* no power available.*/
    {   20,  24.0f,  0.0f,       0.0f,         0.0f },
    /* find perimeter wire.*/
    {   20,  24.0f,  0.05f,      0.05f,        0.0f },
    /* perimeter wire found.*/
    {   20,  24.0f,  0.0f,       0.0f,         0.0f },
    /* charging in base station.*/
    {   20,  24.0f,  0.0f,       0.0f,      1000.0f },
    {   40,  24.0f,  0.0f,       0.0f,      1000.0f },
                   
    /* 15 */
    {   60,  24.0f,  0.0f,       0.0f,         0.0f },
    {   80,  24.0f,  0.0f,       0.0f,         0.0f },
    {   90,  24.0f,  0.0f,       0.0f,         0.0f },
    {  100,  24.0f,  0.0f,       0.0f,         0.0f },
    {  100,  24.0f,  0.0f,       0.0f,         0.0f },
                   
    /* 20 */
    {  100,  24.0f,  0.0f,       0.0f,         0.0f },
    {  100,  24.0f,  0.0f,       0.0f,         0.0f },
    /* leaving base station. cutting backwards.*/
    {  100,  24.0f, -1.570795f, -1.570795f,    0.0f },
    {   -1,   0.0f,  0.0f,       0.0f,         0.0f },
};
