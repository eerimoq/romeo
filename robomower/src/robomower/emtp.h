/**
 * @file robomower/emtp.h
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

#ifndef __ROBOMOWER_EMTP_H__
#define __ROBOMOWER_EMTP_H__

#include "simba.h"
#include "robomower.h"

/* https://en.wikipedia.org/wiki/C0_and_C1_control_codes#DLE

   Data Link Escape

   Cause the following octets to be interpreted as raw data, not as
   control codes or graphic characters. Returning to normal usage
   would be implementation dependent. */
#define DLE 0x10

#define EMTP_MESSAGE_BEGIN DLE

struct emtp_message_header_t {
    uint8_t begin;
    uint8_t type;
    uint16_t size;
} __attribute__((packed));

struct emtp_t {
    chan_t *input_p;
    chan_t *output_p;
    struct {
	int (*callback)(void *arg_p,
			struct emtp_t *emtp_p);
	void *arg_p;
	chan_t *output_p;
    } service;
    struct {
	struct sem_t sem;
	struct chan_t output;
    } internal;
};

int emtp_init(struct emtp_t *emtp_p,
	      chan_t *input_p,
	      chan_t *output_p,
	      chan_t *stream_output_p,
	      int (*message_cb)(void *arg_p,
				struct emtp_t *emtp_p),
	      void *message_cb_arg_p);

int emtp_process(struct emtp_t *emtp_p);

int emtp_write(struct emtp_t *emtp_p,
	       const void *buf_p,
	       size_t size);

int emtp_message_write(struct emtp_t *emtp_p,
		       struct emtp_message_header_t *message_p);

#endif
