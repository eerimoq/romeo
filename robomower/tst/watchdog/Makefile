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

NAME = watchdog_suite
BOARD = linux
SRC += main.c
SLIB_SRC = harness.c
DRIVERS_SRC = uart.c

ROBOMOWER = ../../src
include $(ROBOMOWER)/robomower.mk
SIMBA = ../../../../simba
include $(SIMBA)/make/app.mk
