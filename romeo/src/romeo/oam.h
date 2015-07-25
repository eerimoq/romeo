/**
 * @file romeo/oam.h
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

#ifndef __ROMEO_OAM_H__
#define __ROMEO_OAM_H__

#include "simba.h"
#include "romeo.h"

#define OAM_MESSAGE_TYPE_PING 0
#define OAM_MESSAGE_TYPE_PONG 1

struct oam_message_watchdog_kick_t {
    struct emtp_message_header_t header;
} __attribute__((packed));

#endif
