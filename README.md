# nixie
### Nixie tube

Nixie tubes were used extensivaly in 60's and 70's in various instruments to display digits and other symbols. They are cold cathode tubes made of glass with one anode (a grid) and multiple cathodes, each of which is shaped like the displayed sign.

Cold cathode means that the tube is cool during operation (usually does not exceed 40°C). However, it runs on high voltage (around 200V), so do not attempt to build this project if you don't know exactly what you are doing.

### Clock

- features four tubes to display hours and minutes
- is powered by 9-12V DC
- time is battery backed

### Electronics

- first version will use a PIC16F722A microcontroller
- Microchip MCP7940N real-time clock/calendar
- switching HV supply
- multiplexed display: one anode pulled to HV, BCD driver pulls selected cathode to ground
