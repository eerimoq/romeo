ROBOT
=====

The robot has a two level state machine.

TOP STATE MACHINE
-----------------

![](robot_state_machine.png)

### State descriptions

#### idle
The robot is starting up.

#### panic
The robot is confused and paniced. It will stand still and waiting for
someone to take care of it.

#### cutting
The robot is driving around and cuts the grass.

#### searching_charging_station
The robot is searching for the charging station.

#### in_charging_station
The robot is charging.

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
