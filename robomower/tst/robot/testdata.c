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

FAR const struct testdata_t test_automatic_testdata[] = {
    /* 0 */
    /* cutting forward */
    {  0, { 100,  24.0f,    0.0f }, { 1, 1.570795f,  1.570795f } },
    {  1, {  90,  24.0f,    0.0f }, { 1, 1.570795f,  1.570795f } },
    {  2, {  80,  24.0f,    0.0f }, { 1, 1.570795f,  1.570795f } },
    /* perimeter found. stop.*/
    {  3, {  70, -24.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    /* cutting backwards.*/
    {  4, {  60, -15.0f,    0.0f }, { 1,-1.570795f, -1.570795f } },

    /* 5 */
    /* stop.*/
    {  5, {  50,  12.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    /* cutting rotate.*/
    {  6, {  40,  24.0f,    0.0f }, { 1, 0.235619f, -0.235619f } },
    {  7, {  30,  24.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    /* cutting forward.*/
    {  8, {  30,  24.0f,    0.0f }, { 1, 1.570795f,  1.570795f } },
    {  9, {  30,  24.0f,    0.0f }, { 1, 1.570795f,  1.570795f } },

    /* 10 */
    { 10, {  30,  24.0f,    0.0f }, { 1, 1.570795f,  1.570795f } },
    /* stuck, don't move. */
    { 11, {  -2,  24.0f,  700.0f }, { 1, 0.0f,       0.0f } },
    /* don't stuck, move again. */
    { 12, {  30,  24.0f,    0.0f }, { 1, 1.570795f,  1.570795f } },

    /* no power available. stop. */
    { 13, {  20,  24.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    /* transition gives no output. */
    { 14, {  20,  24.0f,    0.0f }, { 0, 0.0f,       0.0f } },
    /* find perimeter wire. */
    { 15, {  20,  24.0f,    0.0f }, { 1, 0.05f,      0.05f } },
    /* perimeter wire found. */
    { 16, {  20, -24.0f,    0.0f }, { 1, 0.0f,       0.0f } },


    /* 15 */
    /* charging in base station. */
    { 17, {  20,  24.0f, 1000.0f }, { 1, 0.0f,       0.0f } },
    /* transition gives no output. */
    { 28, {  20,  24.0f, 1000.0f }, { 0, 0.0f,       0.0f } },
    { 18, {  40,  24.0f, 1000.0f }, { 1, 0.0f,       0.0f } },

    { 19, {  60,  24.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    { 20, {  80,  24.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    { 21, {  90,  24.0f,    0.0f }, { 1, 0.0f,       0.0f } },

    /* 20 */
    { 22, { 100,  24.0f,    0.0f }, { 1, 0.0f,       0.0f } },
    /* transition gives no output. */
    { 28, { 100,  24.0f,    0.0f }, { 0, 0.0f,       0.0f } },
    /* leaving base station. cutting backwards. */
    { 26, { 100,  24.0f,    0.0f }, { 1,-1.570795f, -1.570795f } },

    /* 25 */
    { 27, {  -1,   0.0f,    0.0f }, { 1, 0.0f,       0.0f } },
};
