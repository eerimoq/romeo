ROBOMOWER
=========

RoboMower is a lawn mower DIY project, inspired by Ardumower. It uses
the Simba microkernel and build system instead of the Arduino IDE and
libraries.

The goal of the project is to create a simple lawn mower for my tiny
garden of 10 m2, but it will not be limited to that size.

HARDWARE
========

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

SOFTWARE
========

The software is written in c and uses the Simba microkernel and build
framework.

The package robomower, located in src/, contains the source code the
the RoboMower.

Unit tests are in the folder called tst/.

The robot application is in mower/.

The charge station application is in charging_station/.