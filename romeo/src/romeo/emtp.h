/**
 * @file romeo/emtp.h
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

#ifndef __ROMEO_EMTP_H__
#define __ROMEO_EMTP_H__

#include "simba.h"
#include "romeo.h"

/* https://en.wikipedia.org/wiki/C0_and_C1_control_codes#DLE

   Data Link Escape

   Cause the following octets to be interpreted as raw data, not as
   control codes or graphic characters. Returning to normal usage
   would be implementation dependent. */
#define DLE 0x10

#define EMTP_MESSAGE_BEGIN DLE

#define EMTP_MESSAGE_TYPE_PING 0
#define EMTP_MESSAGE_TYPE_PONG 1

struct emtp_message_header_t {
    uint8_t begin;
    uint8_t type;
    uint16_t size;
} __attribute__((packed));

struct emtp_message_ping_t {
    struct emtp_message_header_t header;
};

struct emtp_message_pong_t {
    struct emtp_message_header_t header;
};

struct emtp_t {
    chan_t *input_p;
    chan_t *output_p;
    struct {
	int (*callback)(void *arg_p,
			struct emtp_t *emtp_p,
			struct emtp_message_header_t *header_p);
	void *arg_p;
	chan_t *output_p;
    } service;
    struct {
	struct sem_t sem;
	struct chan_t output;
    } internal;
};

/**
 * Initialize object from given parameters.
 * @param[out] emtp_p Object to initialize.
 * @param[in] input_p Input channel from underlaying layer.
 * @param[in] output_p Output channel to underlaying layer.
 * @param[in] stream_output_p Output channel to stream handler.
 * @param[in] message_cb Message callback. Called when a new
 *                        message is received from underlaying layer.
 * @param[in] message_cb_arg_p User defined argument passed to
 *                             message_cb.
 * @return zero(0) oe negative error code.
 */
int emtp_init(struct emtp_t *emtp_p,
	      chan_t *input_p,
	      chan_t *output_p,
	      chan_t *stream_output_p,
	      int (*message_cb)(void *arg_p,
				struct emtp_t *emtp_p,
				struct emtp_message_header_t *header_p),
	      void *message_cb_arg_p);

/**
 * Try to read next incoming stream byte or message.
 * @param[in] emtp_p Initialized object.
 * @return zero(0) oe negative error code.
 */
int emtp_try_read_input(struct emtp_t *emtp_p);

/**
 * Write data to ubderlaying layer.
 * @param[in] emtp_p Initialized object.
 * @param[in] buf_p Buffer to write.
 * @param[in] size Number of bytes to write.
 * @return Number of bytes written.
 */
ssize_t emtp_write(struct emtp_t *emtp_p,
		   const void *buf_p,
		   size_t size);

/**
 * Write message to ubderlaying layer.
 * @param[in] emtp_p Initialized object.
 * @param[in] message_p Message to send.
 * @return Number of bytes written.
 */
ssize_t emtp_message_write(struct emtp_t *emtp_p,
			   struct emtp_message_header_t *message_p);

#endif
