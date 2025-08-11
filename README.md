# c_RP2040
Embedded C programs for the RP2040 MCU (Raspberry Pi Pico). Demonstrating practical applications and learning resources. 

## Software Dependencies

To build this project, you will need to have the `Raspberry Pi Pico` VS Code extension installed.

## Hardware Dependencies

Check the respective project readme files.

## How to Flash

1. Navigate into the individual project folder (ex. `examples/a_blinky/`).
2. Open the `Raspberry Pi Pico` extension.
3. Import the current project using the `Import Project` feature.
4. Flash the code.
    * Press and hold the `BOOTSEL` button on your Pico.
    * While holding the button, connect the Pico to your computer using the Micro-USB cable.
    * Release the `BOOTSEL` button. The Pico will mount as a mass storage device named `RPI-RP2` or something similar.
    * Run the program in VS Code.
    * The Pico will automatically reboot and start running the program.

## Programs

`Examples`: Example programs demonstrating the basic usage of certain peripherals and sensors.

`Birdsong DDS`: Synthesizing birdsong using Direct Digital Synthesis and Fixed-Point Arithmetic

## Credit

This project builds upon concepts and code found in [Hunter-Adams-RP2040-Demos](https://github.com/vha3/Hunter-Adams-RP2040-Demos).
