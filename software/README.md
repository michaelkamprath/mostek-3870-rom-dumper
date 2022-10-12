# MOSTEK 3870 ROM Dumper Driver
The MK3870 ROM Dumper software in the `mk3870-rom-dumper-driver` directory is built by [PlatformIO](https://platformio.org). Once built, the software is loaded onto the `ATMEGA1284P` in the hardware design via the UART interface. Once running, the software will wait for the user to press any key in the host computer's serial terminal that is connected to the UART. When a key is pressed, the process of dumping the `MK3870` ROM will begin.

The results are printed to the serial terminal in a hex dump format or the [Intel Hex](https://en.wikipedia.org/wiki/Intel_HEX) format. To print the dump in the Intel Hex format, set the preprocessor macro `INTEL_HEX_FORMAT` to the value of `1`. 


## Reverse Engineering Clock Timing
The exact clock timing required to simulate a given set of instructions using the `MK3870`'s `TEST` pin can be captured by doing a clock timing log dump. This will happen if the preprocessor macro `LOG_CLOCK_TIMING` has its value set to 1. This should be down using a `MOSTEK 38P70` that allows a `2716` EPROM to be attached to it. In this EPROM, byte code for a know sequence of instructions and byte values should be loaded. And example of this is provided in the `reverse-enginering-sample`. 
