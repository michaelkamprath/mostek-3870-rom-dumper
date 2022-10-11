# MOSTEK 3870 ROM Dumper
This project is used to dump the ROM of a vintage MOSTEK 3870 microcontroller. A ATMEGA1284P microcontroller is used to drive the MK3870 in a manner where the ROM contents can be captured. The `schematic` directory contains the KiCad drawings of the electrical circuit, and the `software` director contains the software to be loaded onto the ATMEGA1284P. [PlatformIO](https://platformio.org) is used to compile the AVR code. 

This project was heavily inspired by [Sean Riddle's solution to the same problem](http://www.seanriddle.com/f8.html). 