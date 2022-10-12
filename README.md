# MOSTEK 3870 ROM Dumper
This project is used to dump the ROM of a vintage MOSTEK 3870 microcontroller. A ATMEGA1284P microcontroller is used to drive the MK3870 in a manner where the ROM contents can be captured. The `schematic` directory contains the KiCad drawings of the electrical circuit, and the `software` director contains the software to be loaded onto the ATMEGA1284P. [PlatformIO](https://platformio.org) is used to compile the AVR code. 

This project was heavily inspired by [Sean Riddle's solution to the same problem](http://www.seanriddle.com/f8.html).

## Usage
To use, simply build the circuit provided in the `schematic` directory, and then run the software as described in the `software` directory. It is a good idea to test with a `MOSTEK 38P70` that has a EPROM attached containing known contents so as to verify the system is working before attaching the target `MK3870` with the unknown ROM contents.

## License
Use as you see fit. Attribution would be appreciated.