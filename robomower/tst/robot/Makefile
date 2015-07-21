#
# @file filter/Makefile
# @version 0.1
#
# @section License
# Copyright (C) 2015, Erik Moqvist
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# This file is part of the RoboMower project.
#

NAME = robot_suite
BOARD = linux
SRC += main.c \
	perimeter_wire_rx_stub.c \
	motor_stub.c \
	power_stub.c \
	testdata.c
SLIB_SRC = harness.c
DRIVERS_SRC = uart.c
ROBOMOWER_SRC = robot.c \
	robot_states.c \
	robot_transitions.c \
	movement.c \
	controller.c

CFLAGS_EXTRA = -DCUTTING_STATE_BACKWARDS_TICKS=2 \
	-DCUTTING_STATE_ROTATING_TICKS=2

ROBOMOWER = ../../src
include $(ROBOMOWER)/robomower.mk
SIMBA = ../../../../archive/simba
include $(SIMBA)/make/app.mk
