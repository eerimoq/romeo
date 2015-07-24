/**
 * @file robot.c
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
#include "robot.h"

#define FOLLOW_KP  1.0
#define FOLLOW_KI  0.0
#define FOLLOW_KD -0.1

FS_COMMAND_DEFINE("/robot/start", robot_cmd_start);
FS_COMMAND_DEFINE("/robot/stop", robot_cmd_stop);
FS_COMMAND_DEFINE("/robot/status", robot_cmd_status);
FS_COMMAND_DEFINE("/robot/mode/set", robot_cmd_mode_set);
FS_COMMAND_DEFINE("/robot/manual/movement/set", robot_cmd_manual_movement_set);
FS_COMMAND_DEFINE("/robot/watchdog/kick", robot_cmd_watchdog_kick);

FS_COUNTER_DEFINE(robot_process);
FS_COUNTER_DEFINE(number_of_state_transitions);

FS_PARAMETER_DEFINE("/robot/parameters/watchdog/enabled", robot_parameter_watchdog_enabled, int, 1);

static struct robot_t robot;

/* Modes as strings. */
static FAR const char manual_string[] = "manual";
static FAR const char automatic_string[] = "automatic";

FAR const char FAR *mode_as_string[] = {
    manual_string,
    automatic_string
};

/* States as strings. */
static FAR const char idle_string[] = "idle";
static FAR const char starting_string[] = "starting";
static FAR const char cutting_string[] = "cutting";
static FAR const char searching_for_base_station_string[] = "searching_for_base_station";
static FAR const char in_base_station_string[] = "in_base_station";

FAR const char FAR *state_as_string[] = {
    idle_string,
    starting_string,
    cutting_string,
    searching_for_base_station_string,
    in_base_station_string
};

/* Cutting states as strings. */
static FAR const char cutting_forward_string[] = "forward";
static FAR const char cutting_backwards_string[] = "backwards";
static FAR const char cutting_rotating_string[] = "rotating";

FAR const char FAR *cutting_state_as_string[] = {
    cutting_forward_string,
    cutting_backwards_string,
    cutting_rotating_string
};

/* Searching for base station states as strings. */
static FAR const char searching_state_searching_for_perimeter_wire_string[] =
    "searching_for_perimeter_wire";
static FAR const char searching_state_following_perimeter_wire_string[] =
    "following_perimeter_wire";

FAR const char FAR *searching_state_as_string[] = {
    searching_state_searching_for_perimeter_wire_string,
    searching_state_following_perimeter_wire_string,
};

int robot_cmd_start(int argc,
		    const char *argv[],
		    void *out_p,
		    void *in_p)
{
    robot_manual_start();

    return (0);
}

int robot_cmd_stop(int argc,
		   const char *argv[],
		   void *out_p,
		   void *in_p)
{
    robot_manual_stop();

    return (0);
}

int robot_cmd_status(int argc,
		     const char *argv[],
		     void *out_p,
		     void *in_p)
{
    UNUSED(in_p);
    struct time_t time;

    std_fprintf(out_p, FSTR("mode = "));
    std_fprintf(out_p, mode_as_string[robot.mode]);
    std_fprintf(out_p, FSTR("\r\nstate = /"));
    std_fprintf(out_p, state_as_string[robot.state.current]);

    switch (robot.state.current) {
    case ROBOT_STATE_CUTTING:
	std_fprintf(out_p, FSTR("/"));
	std_fprintf(out_p, cutting_state_as_string[robot.substate.cutting.state]);
	break;
    case ROBOT_STATE_SEARCHING_FOR_BASE_STATION:
	std_fprintf(out_p, FSTR("/"));
	std_fprintf(out_p, searching_state_as_string[robot.substate.searching.state]);
	break;
    }

    time.seconds = 0;
    time.nanoseconds = PROCESS_PERIOD_NS;
    std_fprintf(out_p,
		FSTR("\r\nprocess period = %d ticks\r\n"
		     "processing time = %d ticks\r\n"
		     "perimeter signal level = %d\r\n"),
		(int)T2ST(&time),
		robot.debug.processing_time,
		(int)perimeter_wire_rx_get_cached_signal(&robot.perimeter));

    if (robot.mode == ROBOT_MODE_MANUAL) {
	std_fprintf(out_p,
		    FSTR("speed = %d.%u m/s\r\n"
			 "omega = %d.%u rad/s\r\n"),
		    (int)(robot.manual.speed),
		    ((unsigned int)(robot.manual.speed * 100.0f)) % 100,
		    (int)(robot.manual.omega),
		    ((unsigned int)(robot.manual.omega * 100.0f)) % 100);
    }

    return (0);
}

int robot_cmd_mode_set(int argc,
		       const char *argv[],
		       void *out_p,
		       void *in_p)
{
    UNUSED(in_p);

    if (argc != 2) {
	std_fprintf(out_p, FSTR("Usage: set {manual,automatic}\r\n"));
	return (1);
    }

    if (std_strcmp(argv[1], FSTR("manual")) == 0) {
	robot.mode = ROBOT_MODE_MANUAL;
    } else if (std_strcmp(argv[1], FSTR("automatic")) == 0) {
	robot.mode = ROBOT_MODE_AUTOMATIC;
    } else {
	std_fprintf(out_p, FSTR("Bad mode '%s'\r\n"), argv[1]);
    }

    return (0);
}

int robot_cmd_manual_movement_set(int argc,
				  const char *argv[],
				  void *out_p,
				  void *in_p)
{
    UNUSED(in_p);

    long speed;
    long omega;

    std_strtol(argv[1], &speed);
    std_strtol(argv[2], &omega);

    robot.manual.speed = ((float)speed) / 100.0f;
    robot.manual.omega = ((float)omega) / 100.0f;

    return (0);
}

int robot_cmd_watchdog_kick(int argc,
			    const char *argv[],
			    void *out_p,
			    void *in_p)
{
    return (watchdog_kick(&robot.watchdog));
}

static void timer_callback(void *arg_p)
{
    struct thrd_t *thrd_p = arg_p;

    thrd_resume_irq(thrd_p, 0);
}

/**
 * Handle state transition.
 * @return zero(0) or negative error code
 */
static int handle_state_transition(struct robot_t *robot_p)
{
    int current = robot_p->state.current;
    int next = robot_p->state.next;
    transition_callback_t callback;
    state_callback_t state_callback;

    callback = NULL;

    switch (current) {

    case ROBOT_STATE_IDLE:
	switch (next) {
	case ROBOT_STATE_STARTING:
	    callback = robot_transition__idle__starting;
	    break;
	default:
	    break;
	}
	break;

    case ROBOT_STATE_STARTING:
	switch (next) {
	case ROBOT_STATE_CUTTING:
	    callback = robot_transition__starting__cutting;
	    break;
	case ROBOT_STATE_IN_BASE_STATION:
	    callback = robot_transition__starting__in_base_station;
	    break;
	default:
	    break;
	}
	break;

    case ROBOT_STATE_CUTTING:
	switch (next) {
	case ROBOT_STATE_SEARCHING_FOR_BASE_STATION:
	    callback = robot_transition__cutting__searching_for_base_station;
	    break;
	case ROBOT_STATE_IDLE:
	    callback = robot_transition__cutting__idle;
	    break;
	default:
	    break;
	}
	break;

    case ROBOT_STATE_SEARCHING_FOR_BASE_STATION:
	switch (next) {
	case ROBOT_STATE_IN_BASE_STATION:
	    callback = robot_transition__searching_for_base_station__in_base_station;
	    break;
	case ROBOT_STATE_IDLE:
	    callback = robot_transition__searching_for_base_station__idle;
	    break;
	default:
	    break;
	}
	break;

    case ROBOT_STATE_IN_BASE_STATION:
	switch (next) {
	case ROBOT_STATE_CUTTING:
	    callback = robot_transition__in_base_station__cutting;
	    break;
	case ROBOT_STATE_IDLE:
	    callback = robot_transition__in_base_station__idle;
	    break;
	default:
	    break;
	}
	break;

    default:
	break;
    }

    /* Bad state transition. */
    if (callback == NULL) {
	std_printk(STD_LOG_ERR,
		   FSTR("bad state transistion %d -> %d"),
		   current,
		   next);
	return (-1);
    }

    std_printk(STD_LOG_NOTICE,
	       FSTR("state transistion %d -> %d"),
	       current,
	       next);

    /* Call the transition callback. */
    state_callback = callback(robot_p);
    if (state_callback == NULL) {
	std_printk(STD_LOG_NOTICE,
		   FSTR("failed state transistion %d -> %d"),
		   current,
		   next);
	return (-1);
    }

    /* Update state on successful state transition. */
    robot_p->state.current = next;
    robot_p->state.callback = state_callback;

    FS_COUNTER_INC(number_of_state_transitions, 1);

    return (0);
}

static int robot_process(struct robot_t *robot_p)
{
    FS_COUNTER_INC(robot_process, 1);

    /* Stop the robot if the watchdog is enabled and has not been
       kicked recently. */
    if (FS_PARAMETER(robot_parameter_watchdog_enabled) == 1) {
	if (watchdog_tick(&robot_p->watchdog) == 0) {
	    robot_p->state.next = ROBOT_STATE_IDLE;
	}
    }

    /* Execute robot state machine. */
    if (robot_p->state.current == robot_p->state.next) {
	return (robot_p->state.callback(robot_p));
    } else {
	return (handle_state_transition(robot_p));
    }
}

int robot_init()
{
    robot.state.current = ROBOT_STATE_IDLE;
    robot.state.next = ROBOT_STATE_IDLE;
    robot.state.callback = robot_state_idle;
    robot.mode = ROBOT_MODE_AUTOMATIC;

    motor_init(&robot.left_motor,
	       &pin_d2_dev,
	       &pin_d3_dev,
	       &pwm_d10_dev,
	       &adc_0_dev,
	       &pin_a2_dev);

    motor_init(&robot.right_motor,
	       &pin_d5_dev,
	       &pin_d6_dev,
	       &pwm_d11_dev,
	       &adc_0_dev,
	       &pin_a3_dev);

    perimeter_wire_rx_init(&robot.perimeter,
			   &adc_0_dev,
			   &pin_a0_dev);

    power_init(&robot.power,
	       &adc_0_dev,
	       &pin_a1_dev);

    controller_pid_init(&robot.follow_pid_controller,
			FOLLOW_KP,
			FOLLOW_KI,
			FOLLOW_KD);

    watchdog_init(&robot.watchdog,
		  WATCHDOG_TIMEOUT_TICKS);

    return (0);
}

void *robot_entry(void *arg_p)
{
    struct time_t timeout;
    struct time_t start_time;

    robot.self_p = thrd_self();
    thrd_set_name("robot");

    /* Start the robot periodic timer with a 50ms period. */
    timeout.seconds = 0;
    timeout.nanoseconds = PROCESS_PERIOD_NS;

    timer_set(&robot.ticker,
	      &timeout,
	      timer_callback,
	      robot.self_p,
	      TIMER_PERIODIC);

    /* Robot main loop. */
    while (1) {
	/* Timer callback resumes this thread. */
	thrd_suspend(NULL);

	time_get(&start_time);
	robot_process(&robot);
	time_get(&timeout);
	robot.debug.processing_time = (timeout.seconds - start_time.seconds);
    }

    return (0);
}

int robot_manual_start()
{
    robot.state.next = ROBOT_STATE_STARTING;

    watchdog_start(&robot.watchdog);

    return (0);
}

int robot_manual_stop()
{
    robot.state.next = ROBOT_STATE_IDLE;

    watchdog_stop(&robot.watchdog);

    return (0);
}
