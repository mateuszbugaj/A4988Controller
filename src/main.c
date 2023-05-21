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

void handle_speed_command(uint16_t speed) {
    a4988_set_speed(&driver, speed);
}


int main(void) {
    clock_prescale_set(clock_div_1);

    // Initialize A4988 driver
    a4988_init(&driver);
    a4988_set_speed(&driver, 120);

    // Initialize USART
    usart_init();
    parser_init(handle_speed_command);

    int timestamp = 0;
    GPIOPin pinLED = { .port = &PORTB, .pin = PB0 };
    gpio_pin_direction(pinLED, OUTPUT);
    usart_print("Start.");
    while (1) {
        gpio_pin_write(pinLED, HIGH);
        _delay_ms(500);

        gpio_pin_write(pinLED, LOW);
        _delay_ms(500);

    }

    return 0;
}

ISR(TIMER1_COMPA_vect) {
    // This ISR will be called each time Timer1 matches the value in OCR1A
    a4988_step(&driver);
}