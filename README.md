# nixie
## Nixie tube

Nixie tubes were used extensivaly in 60's and 70's in various instruments to display digits and other symbols. They are cold cathode tubes made of glass with one anode (a grid) and multiple cathodes, each of which is shaped like the displayed sign.

Cold cathode means that the tube is cool during operation (usually does not exceed 40°C). However, it runs on high voltage (around 200V), so do not attempt to build this project if you don't know exactly what you are doing.

## Clock

- features four tubes to display hours and minutes
- has an alarm
- is powered by 12V DC
- easily reprogrammable with a PICkit3
- can communicate via serial
- time is battery backed

## Electronics

- Microchip PIC16F722A/723A microcontroller
- Microchip MCP7940N real-time clock/calendar
- switching HV supply
- multiplexed display: one anode pulled to HV, BCD driver pulls selected cathode to ground


## I/O

### Inputs

- rotary encoder with a momentary switch
- push-button (optional)
- anode voltage trimmer

### Outputs

- 4 nixie tubes with digits 0-9
- two LEDs in the upper left-hand corner
- buzzer connector (12V)

### Serial port

There is an optional 3-pin header with GND, TX and RX available should there be a need for interfacing with other equipment.