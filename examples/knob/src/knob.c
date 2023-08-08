#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>

#include "../inc/gpio.h"
#include "../inc/a4988.h"

A4988 motor1 = {
    .step = { .port = &PORTB, .pin = PB1 },
    .dir = { .port = &PORTB, .pin = PB2 },
    .sleep = { .port = &PORTB, .pin = PB6},
    .ms1 = { .port = &PORTD, .pin = PD6},
    .ms2 = { .port = &PORTD, .pin = PD5},
    .ms3 = { .port = &PORTB, .pin = PB7}
};

GPIOPin pinLED = { .port = &PORTB, .pin = PB0 };

bool x = true;

int main(void) {
    clock_prescale_set(clock_div_1);

    // Initialize A4988 motor1
    a4988_init(&motor1);

    gpio_pin_direction(pinLED, OUTPUT);
    a4988_set_speed(&motor1, 100);
    a4988_set_microstepping(&motor1, 4);
    while (1) {
        if(a4988_is_moving(&motor1)){
            gpio_pin_write(pinLED, HIGH);
        } else {
            gpio_pin_write(pinLED, LOW);

            _delay_ms(1000);
            
            if(x){
                a4988_set_angle(&motor1, 300);
                x = false;
            } else {
                a4988_set_angle(&motor1, 0);
                x = true;
            }
        }
    }

    return 0;
}

ISR(TIMER0_COMPA_vect) {
    a4988_step(&motor1);
}