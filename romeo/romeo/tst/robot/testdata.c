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
 * This file is part of the Romeo project.
 */

#include "simba.h"
#include "romeo.h"
#include "testdata.h"

FAR const struct testdata_t test_automatic_testdata[] = {
    /* 0 */
    /* cutting forward */
    {  0, { 100,  50.0f,    0.0f }, { 1, 2.500000f,  2.500000f } },
    {  1, {  90,  50.0f,    0.0f }, { 1, 2.500000f,  2.500000f } },
    {  2, {  80,  50.0f,    0.0f }, { 1, 2.500000f,  2.500000f } },
    /* perimeter found. stop.*/
    {  3, {  70,  -6.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    /* cutting backwards.*/
    {  4, {  60,  -2.0f,    0.0f }, { 1,-2.500000f, -2.500000f } },

    /* 5 */
    /* stop.*/
    {  5, {  50,   9.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    /* cutting rotate.*/
    {  6, {  40,  50.0f,    0.0f }, { 1, 1.000000f, -1.000000f } },
    {  7, {  30,  50.0f,    0.0f }, { 1, 1.000000f, -1.000000f } },
    /* cutting forward.*/
    {  8, {  30,  50.0f,    0.0f }, { 1, 2.500000f,  2.500000f } },
    {  9, {  30,  50.0f,    0.0f }, { 1, 2.500000f,  2.500000f } },

    /* 10 */
    { 10, {  30,  50.0f,    0.0f }, { 1, 2.500000f,  2.500000f } },
    /* stuck, don't move. */
    { 11, {  -2,  50.0f,  700.0f }, { 1, 0.0f,       0.0f } },
    /* don't stuck, move again. */
    { 12, {  30,  50.0f,    0.0f }, { 1, 2.500000f,  2.500000f } },

    /* no battery available. stop. */
    { 13, {  20,  50.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    /* transition gives no output. */
    { 14, {  20,  50.0f,    0.0f }, { 0, 0.0f,       0.0f } },
    /* find perimeter wire. */
    { 15, {  20,  50.0f,    0.0f }, { 1, 2.500000f,      2.500000f } },
    /* perimeter wire found. */
    { 16, {  20,  -3.0f,    0.0f }, { 1, 0.0f,       0.0f } },


    /* 15 */
    /* charging in base station. */
    { 17, {  20,  50.0f, 1000.0f }, { 1, 0.0f,       0.0f } },
    /* transition gives no output. */
    { 28, {  20,  50.0f, 1000.0f }, { 0, 0.0f,       0.0f } },
    { 18, {  40,  50.0f, 1000.0f }, { 1, 0.0f,       0.0f } },

    { 19, {  60,  50.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    /* Bad test data. The robot should drive in reverse leaving the
       base station, not retate. */
    { 20, {  80,  50.0f,    0.0f }, { 1, -3.141590f, 3.141590f } },
    { 21, {  90,  50.0f,    0.0f }, { 1, -3.141590f, 3.141590f  } },

    /* 20 */
    { 22, { 100,  50.0f,    0.0f }, { 1, -3.141590f, 3.141590f } },
    /* transition gives no output. */
    { 28, { 100,  50.0f,    0.0f }, { 0, -3.141590f, 3.141590f } },
    /* leaving base station. cutting backwards. */
    { 26, { 100,  50.0f,    0.0f }, { 1, -3.141590f, 3.141590f } },

    /* 25 */
    { 27, {  -1,   0.0f,    0.0f }, { 1, 0.0f,       0.0f } },
};
