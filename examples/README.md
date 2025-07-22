# RP2040 Examples

## RP2040 Blinky & "Hello, World!"

This is a simple "getting started" project for the Raspberry Pi Pico (RP2040). It demonstrates two fundamental operations. Blinking the onboard LED using GPIO control and printing Hello, world! to a USB serial console.

### Hardware Requirements

* A Raspberry Pi Pico or any other RP2040-based board.
* A Micro-USB cable.

The code uses the default onboard LED pin (GPIO 25) for the Raspberry Pi Pico.

### Expected Behavior

* The onboard LED will start blinking, turning on for 250ms and off for 250ms.
* If you connect to the Pico's USB serial port with a terminal emulator (e.g., PuTTY, minicom, screen, serial monitor extension), you will see the message Hello, world! printed twice per second.

## RP2040 GPIO Interrupt

This project demonstrates how to use GPIO interrupts on the Raspberry Pi Pico (RP2040) to control an LED. Specifically, it configures an interrupt on a rising edge of a GPIO pin to toggle the state of an LED.

### Hardware Requirements

* A Raspberry Pi Pico or any other RP2040-based board.
* A Micro-USB cable.
* A 1k Ohms resistor that wires GPIO 3 and GPIO 6.

The code uses the default onboard LED pin (GPIO 25) for the Raspberry Pi Pico.

### Expected Behavior

* The onboard LED will start blinking, turning on for 1000ms and off for 1000ms.

## RP2040 Direct Digital Synthesis (DDS) with SPI DAC and Timer Interrupt

This project demonstrates the implementation of Direct Digital Synthesis (DDS) on the Raspberry Pi Pico (RP2040) to generate a sine wave. The generated digital samples are then sent via an SPI interface to an external Digital-to-Analog Converter (DAC) to produce an analog sine wave output. A hardware timer interrupt is used to control the sampling rate and provide a program heartbeat.

### Hardware Requirements

* A Raspberry Pi Pico or any other RP2040-based board.
* A Micro-USB cable.
* An external SPI-compatible Digital-to-Analog Converter (DAC) module (eg. MCP4822).
* A 3.5mm audio jack connected to the DAC output.
* A speaker connected to the audio jack.

The code uses the default onboard LED pin (GPIO 25) for the Raspberry Pi Pico as a program heartbeat indicator.

### Expected Behavior

* The onboard LED (GPIO 25) will blink at a regular interval (determined by DELAY), serving as a visual heartbeat for the program.
* The connected SPI DAC will output a continuous sine wave.
* An audible sine wave (tone) will be produced from the speaker connected to the audio jack.