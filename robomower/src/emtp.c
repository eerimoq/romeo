/**
 * @file emtp.c
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

/**
 * Output channel write callback. Ensures message integrity.
 */
static ssize_t output_channel_write_cb(chan_t *chan_p,
                                       void *buf_p,
                                       size_t size)
{
    struct emtp_t *emtp_p = container_of(chan_p, struct emtp_t, internal.output);

    sem_get(&emtp_p->internal.sem, NULL);

    chan_write(emtp_p->output_p, buf_p, size);

    sem_put(&emtp_p->internal.sem, 1);

    return (size);
}

int emtp_init(struct emtp_t *emtp_p,
              chan_t *input_p,
              chan_t *output_p,
              chan_t *stream_output_p,
              int (*message_cb)(void *arg_p,
                                struct emtp_t *emtp_p),
              void *message_cb_arg_p)
{
    emtp_p->input_p = input_p;
    emtp_p->output_p = output_p;

    emtp_p->service.callback = message_cb;
    emtp_p->service.arg_p = message_cb_arg_p;
    emtp_p->service.output_p = stream_output_p;

    sem_init(&emtp_p->internal.sem, 1);
    chan_init(&emtp_p->internal.output,
              NULL,
              (ssize_t (*)(chan_t *, const void *, size_t))output_channel_write_cb,
              NULL);

    return (0);
}

int emtp_process(struct emtp_t *emtp_p)
{
    char c;

    chan_read(emtp_p->input_p, &c, sizeof(c));

    if (c == EMTP_MESSAGE_BEGIN) {
        emtp_p->service.callback(emtp_p->service.arg_p, emtp_p);
    } else {
	chan_write(emtp_p->service.output_p, &c, sizeof(c));
    }

    return (0);
}

ssize_t emtp_write(struct emtp_t *emtp_p,
                   const void *buf_p,
                   size_t size)
{
    return (chan_write(&emtp_p->internal.output, buf_p, size));
}

ssize_t emtp_message_write(struct emtp_t *emtp_p,
                           struct emtp_message_header_t *message_p)
{    return (chan_write(&emtp_p->internal.output,
		       message_p,
		       message_p->size));
}
