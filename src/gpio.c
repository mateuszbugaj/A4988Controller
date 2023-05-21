#include "../inc/gpio.h"

void gpio_pin_direction(GPIOPin pin, PinDirection direction) {
    if (direction == OUTPUT) {
        *(pin.port - 1) |= (1 << pin.pin); // DDRx is one address below PORTx
    } else {
        *(pin.port - 1) &= ~(1 << pin.pin);
    }
}

void gpio_pin_write(GPIOPin pin, PinLevel level) {
    if (level == HIGH) {
        *(pin.port) |= (1 << pin.pin);
    } else {
        *(pin.port) &= ~(1 << pin.pin);
    }
}

PinLevel gpio_pin_read(GPIOPin pin) {
    return (*(pin.port - 2) & (1 << pin.pin)) ? HIGH : LOW; // PINx is two addresses below PORTx
}
