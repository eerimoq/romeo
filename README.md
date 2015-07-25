OVERVIEW
========

RoboMower is a lawn mower DIY project, inspired by Ardumower. It uses
the Simba microkernel and build system instead of the Arduino IDE and
libraries.

The goal of the project is to create a simple lawn mower for my tiny
garden of 10 m2, but it will not be limited to that size.

![](doc/robot.jpg)

CONNECTIVITY
============

Control and monitor the Robot and the Base station from a device of
your choice, normally a PC. There is also a smartphone application to
manually control the robot.

![](doc/connectivity.jpg)

CONTROLLER SMARTPHONE APP
-------------------------
The controller smartphone application can control the speed and
angular velocity of the robot. The application connects to the robot
using the same bluetooth interface as the base station, so only one of
the two can be connected at a time.

![](doc/controller_smartphone_app.jpg)

HARDWARE
========

ROBOT SCHEMATICS
----------------
![](doc/robot_schematics.jpg)

BASE STATION SCHEMATICS
-----------------------
![](doc/base_station_schematics.jpg)

SOFTWARE
========

The software is written in c and uses the Simba microkernel and build
framework.

Folder structure:

    -- {root}
       +-- app/
       |   +-- base_station/             - base station application
       |   +-- robot/                    - robot application
       +-- doc/                          - documenatation
       +-- LICENSE
       +-- Makefile
       +-- README.md
       +-- robomower/                    - robomower package
           +-- src/                      - source code
           +-- tst/                      - test suites
           +-- tools/                    - various tools

ROBOT STATE MACHINE
-------------------
The robot has a two level state machine.

### Top level state machine
![](doc/robot_state_machine.png)

#### State descriptions

The grey state is not part of the software state machine. It is
present to give a clear picture of what happens when the power is
turned on.

In the light blue states the robot can drive around in the garden.

In the white states the robot is standing still.

##### idle
The robot is idle, waiting to be started manually.

##### starting
The robot is starting.

##### cutting
The robot is driving around and cuts the grass.

##### searching_for_base_station
The robot is searching for the base station.

##### in_base_station
The robot is in the base station.

### Cutting state machine
This is the state diagram for the cutting state.

![](doc/robot_state_machine_cutting.png)

#### State descriptions

##### forward
The robot is driving forward.

##### backwards
The robot is driving backwards.

##### rotating
The robot is rotating.

### Searching for base station state machine
This is the state diagram for the searching_for_base_station state.

![](doc/robot_state_machine_searching.png)

#### State descriptions

##### searching_for_perimeter_wire
The robot is searching for the perimeter wire.

##### following_perimeter_wire
The robot is following the perimeter wire to the base station.

PARTS
-----

### Audio amplifier
http://www.ebay.com/sch/i.html?_odkw=lm386&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR0.TRC0.H0.Xlm386+audio.TRS0&_nkw=lm386+audio&_sacat=0

### Inductor
http://www.ebay.com/itm/20pcs-Radial-Inductor-100mH-104-8mm-x-10mm-10-/250796992219?pt=LH_DefaultDomain_0&hash=item3a64aa66db

### Motor controller
http://www.ebay.com/itm/New-L298N-DC-Stepper-Motor-Driver-Module-Dual-H-Bridge-Control-Board-for-Arduino-/301663970473?pt=LH_DefaultDomain_0&hash=item463c92e4a9

### Current sensor
http://www.ebay.com/itm/New-design-30A-range-Current-Sensor-Module-ACS712-Module-Arduino-module-/310506962976?pt=LH_DefaultDomain_0&hash=item484ba84020

### Chassi
http://www.ikea.com/se/sv/catalog/products/80131672/
