/**
 * @file main.c
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
#include <arpa/inet.h>

static struct sem_t message_callback_sem;

static int message_callback(void *arg_p,
			    struct emtp_t *emtp_p,
                            struct emtp_message_header_t *header_p)
{

    std_printk(STD_LOG_NOTICE, FSTR("message callback"));

    BTASSERT(header_p->type == EMTP_MESSAGE_TYPE_PING);
    BTASSERT(htons(header_p->size) == sizeof(struct emtp_message_ping_t));

    sem_put(&message_callback_sem, 1);

    return (0);
}

static int test_message(struct harness_t *harness_p)
{
    struct emtp_t emtp;
    struct queue_t input;
    struct queue_t output;
    struct queue_t stream_output;
    char input_buf[32];
    char output_buf[32];
    char stream_output_buf[32];
    struct emtp_message_ping_t ping;

    BTASSERT(queue_init(&input,
			input_buf,
			sizeof(input_buf)) == 0);
    BTASSERT(queue_init(&output,
			output_buf,
			sizeof(output_buf)) == 0);
    BTASSERT(queue_init(&stream_output,
			stream_output_buf,
			sizeof(stream_output_buf)) == 0);

    BTASSERT(emtp_init(&emtp,
		       &input,
		       &output,
		       &stream_output,
		       message_callback,
		       NULL) == 0);

    /* Write a message on the input channel. */
    ping.header.begin = EMTP_MESSAGE_BEGIN;
    ping.header.type = EMTP_MESSAGE_TYPE_PING;
    ping.header.size = sizeof(ping);
    BTASSERT(chan_write(&input, &ping, sizeof(ping)) == sizeof(ping));

    BTASSERT(emtp_try_read_input(&emtp) == 0);

    /* Wait for signal from message callback. */
    sem_get(&message_callback_sem, NULL);

    /* Write a message on the internal input channel. */
    ping.header.type = EMTP_MESSAGE_TYPE_PING;
    ping.header.size = sizeof(ping);
    BTASSERT(emtp_message_write(&emtp, &ping.header) == sizeof(ping));

    /* Read the message from the output channel and verify its
       contents. */
    ping.header.begin = ~EMTP_MESSAGE_BEGIN;
    ping.header.type = -1;
    ping.header.size = 0;
    BTASSERT(chan_read(&output, &ping, sizeof(ping)) == sizeof(ping));
    BTASSERT(ping.header.begin == EMTP_MESSAGE_BEGIN);
    BTASSERT(ping.header.type == EMTP_MESSAGE_TYPE_PING);
    BTASSERT(htons(ping.header.size) == sizeof(ping));

    return (0);
}

static int test_stream(struct harness_t *harness_p)
{
    struct emtp_t emtp;
    struct queue_t input;
    struct queue_t output;
    struct queue_t stream_output;
    char input_buf[32];
    char output_buf[32];
    char stream_output_buf[32];
    char buf[16];
    int i;

    BTASSERT(queue_init(&input,
			input_buf,
			sizeof(input_buf)) == 0);
    BTASSERT(queue_init(&output,
			output_buf,
			sizeof(output_buf)) == 0);
    BTASSERT(queue_init(&stream_output,
			stream_output_buf,
			sizeof(stream_output_buf)) == 0);

    BTASSERT(emtp_init(&emtp,
		       &input,
		       &output,
		       &stream_output,
		       message_callback,
		       NULL) == 0);

    /* Write some text on the input channel. */
    BTASSERT(chan_write(&input, "foo", sizeof("foo")) == sizeof("foo"));

    /* One byte is processed at a time. */
    for (i = 0; i < sizeof("foo"); i++) {
        BTASSERT(emtp_try_read_input(&emtp) == 0);
    }

    /* Read the text from the stream output channel and verify its
       contents. */
    BTASSERT(chan_read(&stream_output, buf, sizeof("foo")) == sizeof("foo"));
    BTASSERT(strcmp("foo", buf) == 0);

    /* Write some text on the stream input channel. */
    BTASSERT(emtp_write(&emtp, "bar", sizeof("bar")) == sizeof("bar"));

    /* Read the text from the output channel and verify its
       contents. */
    BTASSERT(chan_read(&output, buf, sizeof("bar")) == sizeof("bar"));
    BTASSERT(strcmp("bar", buf) == 0);

    return (0);
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t harness_testcases[] = {
	{ test_message, "test_message" },
	{ test_stream, "test_stream" },
	{ NULL, NULL }
    };

    sys_start();
    uart_module_init();

    sem_init(&message_callback_sem, 0);

    harness_init(&harness);
    harness_run(&harness, harness_testcases);

    return (0);
}
