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
# This file is part of the Romeo project.
#

NAME = robot
VERSION = 0.2.0

BOARD = arduino_mega

SETTINGS_INI = settings.ini

DRIVERS_SRC = uart.c pin.c pwm.c adc.c

ROMEO_SRC = \
	controller.c \
	cutter.c \
	filter.c \
	motor.c \
	movement.c \
	perimeter_wire_rx.c \
	battery.c \
	robot.c \
	robot_states.c \
	robot_transitions.c \
	watchdog.c

ROMEO = ../../romeo/romeo/src
include $(ROMEO)/romeo.mk
SIMBA = ../../../simba
include $(SIMBA)/make/app.mk
