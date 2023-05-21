# Stepper Motor Controller

## Description

This project provides a firmware for controlling a stepper motor using an atmega168 microcontroller and a4988 stepper motor driver.

## Installation

Requirements: avr-gcc, avr-libc, avrdude

1. Clone this repository.
2. Compile the code using the provided Makefile: `make all`
3. Flash the firmware to the atmega168: `make flash`

## Usage

After flashing the firmware to the atmega168, connect the stepper motor to the a4988 driver. The firmware will rotate the stepper motor at a constant speed.