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

#define VERSION_STR "0.1.0"

struct shell_config_t {
    char stack[456];
    struct shell_args_t args;
    struct queue_t input_channel;
    char input_channel_buf[2];
};

FS_COMMAND_DEFINE("/robot/start", robot_cmd_start);
FS_COMMAND_DEFINE("/robot/stop", robot_cmd_stop);
FS_COMMAND_DEFINE("/robot/status", robot_cmd_status);
FS_COMMAND_DEFINE("/robot/mode/set", robot_cmd_mode_set);
FS_COMMAND_DEFINE("/robot/manual/movement/set", robot_cmd_manual_movement_set);
FS_COMMAND_DEFINE("/robot/sensors", robot_cmd_sensors);
FS_COMMAND_DEFINE("/robot/watchdog/kick", robot_cmd_watchdog_kick);

static struct uart_driver_t uart;
static char qinbuf[64];

static struct uart_driver_t uart3;
static char qinbuf3[64];

static struct robot_t robot;
static struct timer_t ticker;
static struct thrd_t *self_p;

static struct shell_config_t shell_default;
static struct shell_config_t shell_bluetooth;

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
    robot_start(&robot);

    return (0);
}

int robot_cmd_stop(int argc,
		   const char *argv[],
		   void *out_p,
		   void *in_p)
{
    robot_stop(&robot);

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
		FSTR("\r\ntick period = %d ticks\r\n"
		     "tick execution time = %d ticks\r\n"
		     "perimeter signal level = %f\r\n"
		     "perimeter signal quality = %f\r\n"
		     "battery voltage = %f V\r\n"
		     "battery energy level = %d%%\r\n"
		     "left motor current = %f\r\n"
		     "right motor current = %f\r\n"
                     "watchdog count = %d\r\n"),
		(int)T2ST(&time),
		robot.debug.tick_time,
		perimeter_wire_rx_get_signal(&robot.perimeter),
		perimeter_wire_rx_get_quality(&robot.perimeter),
		battery_get_battery_voltage(&robot.battery),
		battery_get_stored_energy_level(&robot.battery),
		motor_get_current(&robot.left_motor),
		motor_get_current(&robot.right_motor),
                robot.watchdog.count);

    if (robot.mode == ROBOT_MODE_MANUAL) {
	std_fprintf(out_p,
		    FSTR("speed = %f m/s\r\n"
			 "omega = %f rad/s\r\n"),
		    robot.manual.speed,
		    robot.manual.omega);
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
    float speed_max = movement_get_maximum_speed(&robot.movement);
    float omega_max = movement_get_maximum_omega(&robot.movement);

    std_strtol(argv[1], &speed);
    std_strtol(argv[2], &omega);

    /* Overflow in the movement calculation will occur if both speed
       and omega are close to the maximum. */
    robot.manual.speed = speed_max * (((float)speed) / 100.0f);
    robot.manual.omega = omega_max * (((float)omega) / 100.0f);

    return (0);
}

int robot_cmd_sensors(int argc,
                      const char *argv[],
                      void *out_p,
                      void *in_p)
{
    UNUSED(in_p);

    int i;

    std_fprintf(out_p, FSTR("- Sensor samples -\r\n"));
    std_fprintf(out_p, FSTR("\r\nperimeter wire:\r\n"
                            "  signal = %f\r\n"
                            "  quality = %f\r\n"),
		perimeter_wire_rx_get_signal(&robot.perimeter),
		perimeter_wire_rx_get_quality(&robot.perimeter));

    for (i = 0; i < membersof(robot.perimeter.updated.samples); i++) {
        std_fprintf(out_p, FSTR("  [%2d]: %4d\r\n"),
                    i,
                    robot.perimeter.updated.samples[i]);
    }

    std_fprintf(out_p, FSTR("\r\nleft motor:\r\n"));

    for (i = 0; i < membersof(robot.left_motor.current.updated.samples); i++) {
        std_fprintf(out_p, FSTR("  [%2d]: %4d\r\n"),
                    i,
                    robot.left_motor.current.updated.samples[i]);
    }

    std_fprintf(out_p, FSTR("\r\nright motor:\r\n"));

    for (i = 0; i < membersof(robot.right_motor.current.updated.samples); i++) {
        std_fprintf(out_p, FSTR("  [%2d]: %4d\r\n"),
                    i,
                    robot.right_motor.current.updated.samples[i]);
    }

    std_fprintf(out_p, FSTR("\r\nbattery level:\r\n"));

    for (i = 0; i < membersof(robot.battery.updated.samples); i++) {
        std_fprintf(out_p, FSTR("  [%2d]: %4d\r\n"),
                    i,
                    robot.battery.updated.samples[i]);
    }

    return (0);
}

int robot_cmd_watchdog_kick(int argc,
                            const char *argv[],
                            void *out_p,
                            void *in_p)
{
    return (robot_watchdog_kick(&robot));
}

static void timer_callback(void *arg_p)
{
    struct thrd_t *thrd_p = arg_p;

    thrd_resume_irq(thrd_p, 0);
}

static int init()
{
    sys_start();
    uart_module_init();
    adc_module_init();

    /* Setup UART. */
    uart_init(&uart, &uart_device[0], 38400, qinbuf, sizeof(qinbuf));
    uart_start(&uart);
    sys_set_stdout(&uart.chout);

    uart_init(&uart3, &uart_device[3], 38400, qinbuf3, sizeof(qinbuf3));
    uart_start(&uart3);

    queue_init(&shell_default.input_channel,
               shell_default.input_channel_buf,
               sizeof(shell_default.input_channel_buf));

    queue_init(&shell_bluetooth.input_channel,
               shell_bluetooth.input_channel_buf,
               sizeof(shell_bluetooth.input_channel_buf));

    robot_init(&robot);

    self_p = thrd_self();
    thrd_set_name("robot");

    /* Start the shell. */
    shell_default.args.chin_p = &uart.chin;
    shell_default.args.chout_p = &uart.chout;
    shell_default.args.username_p = "root";
    shell_default.args.password_p = "1234";
    thrd_spawn(shell_entry,
               &shell_default.args,
               20,
               shell_default.stack,
               sizeof(shell_default.stack));

    /* Start the shell. */
    shell_bluetooth.args.chin_p = &uart3.chin;
    shell_bluetooth.args.chout_p = &uart3.chout;
    shell_bluetooth.args.username_p = "root";
    shell_bluetooth.args.password_p = "1234";
    thrd_spawn(shell_entry,
               &shell_bluetooth.args,
               20,
               shell_bluetooth.stack,
               sizeof(shell_bluetooth.stack));

    return (0);
}

int main()
{
    struct time_t timeout;
    struct time_t start_time;

    init();

    std_printf(FSTR("Romeo - robot version " VERSION_STR "\r\n"));

    /* Start the robot periodic timer with a 50ms period. */
    timeout.seconds = 0;
    timeout.nanoseconds = PROCESS_PERIOD_NS;

    timer_set(&ticker,
	      &timeout,
	      timer_callback,
	      self_p,
	      TIMER_PERIODIC);

    /* Robot main loop. */
    while (1) {
	/* Timer callback resumes this thread. */
	thrd_suspend(NULL);

        time_get(&start_time);

        robot_tick(&robot);

        time_get(&timeout);
        robot.debug.tick_time = (timeout.seconds - start_time.seconds);
    }

    return (0);
}
