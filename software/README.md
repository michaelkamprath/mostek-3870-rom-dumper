# MOSTEK 3870 ROM Dumper Driver
The MK3870 ROM Dumper software in the `mk3870-rom-dumper-driver` directory is built by [PlatformIO](https://platformio.org). Once built, the software is loaded onto the `ATMEGA1284P` in the hardware design via the UART interface. Once running, the software will wait for the user to press any key in the host computer's serial terminal that is connected to the UART. When a key is pressed, the process of dumping the `MK3870` ROM will begin.

The results are printed to the serial terminal in a hex dump format or the [Intel Hex](https://en.wikipedia.org/wiki/Intel_HEX) format. To print the dump in the Intel Hex format, set the preprocessor macro `INTEL_HEX_FORMAT` to the value of `1`. 

You should adjust the `ROM_BYTES` preprocessor macro if your `MK3870` has a ROM size other than 2K.

## Reverse Engineering Clock Timing
The exact clock timing required to simulate a given set of instructions using the `MK3870`'s `TEST` pin can be captured by doing a clock timing log dump. This will happen if the preprocessor macro `LOG_CLOCK_TIMING` has its value set to `1`. This should be done using a `MOSTEK 38P70` that allows a `2716` EPROM to be attached to it. Load into this EPROM byte code with a know sequence of instructions and byte values. And example of this is provided in the `reverse-enginering-sample`. The examples are assembled using [**BespokeASM**](https://github.com/michaelkamprath/bespokeasm) and it's [MOSTEK 3870 configuration](https://github.com/michaelkamprath/bespokeasm/tree/main/examples/mostek-3870).
