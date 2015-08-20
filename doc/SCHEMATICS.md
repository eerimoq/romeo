Battery voltage mesurement
--------------------------
             +---------+
             |         |
             |         O R                 +---------+
    Ubat  -------      |                   |         |
            ---        +-------------------+ A1      |
             |         |                   |         |
             |         O Rmeas       Umeas | Arduino |
             |         |                   |         |
             +---------+-------------------+ GND     |
                                           |         |
                                           +---------+

    Ubat:  battery voltage
    Umeas: measurement voltage. Input to arduino.
    Rmeas: measurement resistor
    R:     divider resistor

Choose the resistor valus so the measurement voltage (Umeas) is 4 V
when the battery voltage (Vbat) is 12 V to give some room for
overvoltage. The arduino takes a 10bit sample to get the voltage on
the analog pin. The resolution of the Vbat measurement will be (5000 /
1024) * (12 / 4) = 14.6 mV / sample.

    Ubat = 12V
    Umeas = 4V

Ohms law gives the equation below since the current is the same
through R and Rmeas.

    Ubat / (Rmeas + R) = Umeas / Rmeas

This can be rewritten as:

    R = (Ubat * Rmeas) / Umeas - Rmeas = Rmeas * (Ubat / Umeas - 1)

Inserting chosen valus gives:

    R = 2 * Rmeas

Let's use one 1M ohm resistor for Rmeas and two 1M ohm resistors in
series for R.
