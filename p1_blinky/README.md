# RP2040 Blinky & "Hello, World!"

This is a simple "getting started" project for the Raspberry Pi Pico (RP2040). It demonstrates two fundamental operations:

1.  **GPIO Control**: Blinking the onboard LED.
2.  **STDIO over USB**: Printing "Hello, world!" to a serial console.

## Hardware Requirements

*   A Raspberry Pi Pico or any other RP2040-based board.
*   A Micro-USB cable.

The code uses the default onboard LED pin (`GPIO 25`) for the Raspberry Pi Pico.

## Software Dependencies

To build this project, you will need to have the "Raspberry Pi Pico" VS Code extension installed.

## How to Flash

1.  Press and hold the `BOOTSEL` button on your Pico.
2.  While holding the button, connect the Pico to your computer using the Micro-USB cable.
3.  Release the `BOOTSEL` button. The Pico will mount as a mass storage device named `RPI-RP2`.
4.  Run the program in VS Code.
5.  The Pico will automatically reboot and start running the program.

## Expected Behavior

*   The onboard LED will start blinking, turning on for 250ms and off for 250ms.
*   If you connect to the Pico's USB serial port with a terminal emulator (e.g., PuTTY, minicom, screen, serial monitor extension), you will see the message "Hello, world!" printed twice per second.