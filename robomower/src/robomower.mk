#
# @file robomower/robomower.mk
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

INC += $(ROBOMOWER)

ROBOMOWER_SRC ?= filter.c \
	perimiter_wire_tx.c \
	perimiter_wire_rx.c \
	movement.c \
	cutter.c

SRC += $(ROBOMOWER_SRC:%=$(ROBOMOWER)/%)
