#!/usr/bin/env bash

set -e

dot -Tpng -Gsize=15\! -Gdpi=100 -o robot_state_machine.png robot_state_machine.dot
dot -Tpng -Gsize=3.5\! -Gdpi=100 -o robot_state_machine_cutting.png robot_state_machine_cutting.dot
dot -Tpng -Gsize=3.5\! -Gdpi=100 -o robot_state_machine_searching.png robot_state_machine_searching.dot
