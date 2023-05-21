#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>

#include "../inc/gpio.h"
#include "../inc/a4988.h"

#define DELAY_MS 500
#define NUM_STEPS 200

// Define the A4988 driver pins globally
A4988 driver = {
    .step = { .port = &PORTB, .pin = PB1 },
    .dir = { .port = &PORTB, .pin = PB2 }
};

int main(void) {
    clock_prescale_set(clock_div_1);

    // Initialize A4988 driver
    a4988_init(&driver);

    while (1) {
        // Drive the motor forward
        a4988_set_direction(&driver, 1);
        for (uint16_t i = 0; i < NUM_STEPS; i++) {
            a4988_step(&driver);
            _delay_ms(1);
        }

        _delay_ms(DELAY_MS);

        // Drive the motor in reverse
        a4988_set_direction(&driver, 0);
        for (uint16_t i = 0; i < NUM_STEPS; i++) {
            a4988_step(&driver);
            _delay_ms(1);
        }

        _delay_ms(DELAY_MS);
    }

    return 0;
}
