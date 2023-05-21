#include <util/delay.h>
#include "../inc/a4988.h"

void a4988_init(A4988* driver) {
    // Initialize DIR and STEP pins as outputs
    gpio_pin_direction(driver->dir, OUTPUT);
    gpio_pin_direction(driver->step, OUTPUT);
}

void a4988_set_direction(A4988* driver, uint8_t direction) {
    // Set the direction of the motor
    gpio_pin_write(driver->dir, direction ? HIGH : LOW);
}

void a4988_step(A4988* driver) {
    // Make one step
    gpio_pin_write(driver->step, HIGH);
    _delay_us(500);  // delay for 1 microsecond
    gpio_pin_write(driver->step, LOW);
}
