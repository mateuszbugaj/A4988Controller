#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>

#include "gpio.h"
#include "a4988.h"
#include "parser.h"

A4988 motor1 = {
    .step = { .port = &PORTC, .pin = PC1 },
    .dir = { .port = &PORTB, .pin = PB2 },
    .sleep = { .port = &PORTB, .pin = PB6},
    .ms1 = { .port = &PORTD, .pin = PD6},
    .ms2 = { .port = &PORTD, .pin = PD5},
    .ms3 = { .port = &PORTB, .pin = PB7}
};

GPIOPin pinLED = { .port = &PORTB, .pin = PB0 };

void handle_set_speed_command(uint16_t speed) {
    a4988_set_speed(&motor1, speed);
}

void handle_set_acceleration_command(uint16_t acceleration) {
    a4988_set_acceleration(&motor1, acceleration);
}

void handle_set_angle_command(uint16_t angle) {
    a4988_set_angle(&motor1, angle);
}

void handle_set_microstep_command(uint16_t microstep) {
    a4988_set_microstepping(&motor1, microstep);
}


parser_command_handler_t command_handlers[] = {
    { .command = 'S', .callback = handle_set_speed_command},
    { .command = 'A', .callback = handle_set_angle_command},
    { .command = 'M', .callback = handle_set_microstep_command},
    { .command = 'C', .callback = handle_set_acceleration_command}
};

int main(void) {
    clock_prescale_set(clock_div_1);

    // Initialize A4988 motor1
    a4988_init(&motor1);

    // Initialize USART
    usart_init();
    parser_init(command_handlers, sizeof(command_handlers)/sizeof(command_handlers[0]));

    gpio_pin_direction(pinLED, OUTPUT);
    usart_print("Start\n\r");

    a4988_set_target_speed(&motor1, 100);
    a4988_set_acceleration(&motor1, 40);
    a4988_set_microstepping(&motor1, 4);
    while (1) {
        if(a4988_is_moving(&motor1)){
            gpio_pin_write(pinLED, HIGH);
        } else {
            gpio_pin_write(pinLED, LOW);
        }
    }

    return 0;
}

ISR(TIMER1_COMPA_vect) {
    a4988_step(&motor1);
}