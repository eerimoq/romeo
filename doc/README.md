ROBOT
=====

The robot has a two level state machine.

TOP STATE MACHINE
-----------------

![](robot_state_machine.png)

### State descriptions

#### idle
The robot is idle, waiting to be started manually.

#### cutting
The robot is driving around and cuts the grass.

#### searching_for_base_station
The robot is searching for the base station.

#### in_base_station
The robot is in the base station.

CUTTING STATE MACHINE
---------------------

This is the state diagram for the cutting state.

![](robot_state_machine_cutting.png)

### State descriptions

#### driving_forward
The robot is driving forward.

#### driving_backwards
The robot is driving backwards.

#### rotating
The robot is rotating.
