# Stepper Motor Controller

## Description

This project provides a firmware for controlling a stepper motor using an atmega168 microcontroller and a4988 stepper motor driver.
The firmware is capable of setting a target speed and rotation angle with support for acceleration and deceleration.

## Installation

Requirements: avr-gcc, avr-libc, avrdude

1. Clone this repository.
2. Compile the main library using the provided Makefile: make all.
3. Navigate to the example you want to use (e.g., cd examples/control).
4. Compile the example using the Makefile in its directory: make all.
5. Flash the firmware to the atmega168: make flash.

## Usage

After flashing the firmware to the atmega168, connect the stepper motor to the A4988 driver.

For the knob example, the motor will start rotating at a constant speed as soon as the microcontroller is powered.

For the control example, you can send commands via USART to control the motor's speed and rotation. See the source code in examples/control for a reference on how to form commands.

