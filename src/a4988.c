#include <util/delay.h>
#include "../inc/a4988.h"

#define CLOCK_FREQUENCY 8000000  // Adjust this to match your actual clock frequency
#define PRESCALER 256

void timer1_init() {
    // Configure Timer1
    TCCR1A = 0;
    TCCR1B = (1 << WGM12); // CTC mode

    // Set prescaler to 256
    TCCR1B |= (1 << CS12);

    // Initialize Output Compare Register A to 1 to start with a step immediately
    OCR1A = 1;

    // Enable Output Compare A Match interrupt
    TIMSK1 = (1 << OCIE1A);

    sei();
}

void a4988_init(A4988* driver) {
    // Initialize DIR and STEP pins as outputs
    gpio_pin_direction(driver->dir, OUTPUT);
    gpio_pin_direction(driver->step, OUTPUT);

    // Set default speed
    a4988_set_speed(driver, 200); // Default to 200 steps per second

    timer1_init();
}

void a4988_set_direction(A4988* driver, uint8_t direction) {
    // Set the direction of the motor
    gpio_pin_write(driver->dir, direction ? HIGH : LOW);
}

void a4988_step(A4988* driver) {
    gpio_pin_write(driver->step, HIGH);
    _delay_ms(0.1); // Delay for signal rise
    gpio_pin_write(driver->step, LOW);
}

void a4988_set_speed(A4988* driver, uint16_t speed) {
    if (speed > 0) {
        // Convert speed in steps per second to delay between steps in terms of timer counts
        OCR1A = CLOCK_FREQUENCY / (speed * PRESCALER) - 1;
    }
}
