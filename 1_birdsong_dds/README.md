# Birdsong DDS

## RP2040 Sound Synthesis with DDS

This program demonstrates how to generate a custom sound using a Raspberry Pi Pico (RP2040) microcontroller. It combines Direct Digital Synthesis (DDS) to create a variable-frequency sine wave with an amplitude modulation envelope to produce a birdsong. The output is sent to a Digital-to-Analog Converter (DAC) via SPI, and the process is managed by a state machine within a timer interrupt.

### Hardware Requirements

* A Raspberry Pi Pico or any other RP2040-based board.
* A Micro-USB cable for power and programming.
* A Digital-to-Analog Converter (DAC) that communicates via SPI.
* External audio circuitry to connect the DAC output to a speaker or headphone jack.
* The code is configured to work with a DAC connected to the following GPIO pins:
    * SPI MISO: GPIO 4
    * SPI CS: GPIO 5
    * SPI SCK: GPIO 6
    * SPI MOSI: GPIO 7
    * DAC LDAC (Load DAC): GPIO 8

### Expected Behavior

* The program will generate a repeating, two-part beep sound.
* The first part of the sound is a frequency swoop where the tone starts high, decreases, and then increases again, following a sinusoidal function.
* The second part is a frequency chirp where the tone starts high and increases further, following a parabolic function.
* Both parts of the beep have an amplitude envelope that causes them to fade in and out smoothly (an "attack" and "decay").
* A silent interval separates the two parts of the sound and the repeating cycles.

### Notes

The goal of this project is to trick the `Merlin Bird ID` app into thinking that it's hearing a birdsong of the Northern Cardinal. I was not able to achieve this. I have planned incorporating the following changes to increase the probability of trick the app:
    * Using fixed-point arithmetic for all calculations
