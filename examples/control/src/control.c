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

void handle_set_target_speed_command(uint16_t speed) {
    a4988_set_target_speed(&motor1, speed);
}

void handle_set_acceleration_command(uint16_t acceleration) {
    a4988_set_acceleration(&motor1, acceleration);
}

void handle_set_angle_command(int16_t angle) {
    a4988_set_angle(&motor1, angle);
}

void handle_set_microstep_command(uint16_t microstep) {
    a4988_set_microstepping(&motor1, microstep);
}

void handle_move_by_command(int16_t steps) {
    a4988_move_steps(&motor1, steps);
}

void handle_help_command(){
	usart_print("Help\n\r");
	usart_print("A - Set target angle (0-359)\n\r");
	usart_print("S - Set constant speed in full steps (no acceleration and automatically adjusted for microstepping)\n\r");
	usart_print("T - Set target speed (will use the acceleration)\n\r");
	usart_print("M - Set microstepping ([1, 2, 4, 8] -> [1, 1/2, 1/4, 1/8])\n\r");
	usart_print("C - Set acceleration in full steps per second squared\n\r");
	usart_print("X - Move by of full steps\n\r");
	usart_print("H - This message\n\r");
}

parser_command_handler_t command_handlers[] = {
    { .command = 'S', .callback = handle_set_speed_command},
    { .command = 'A', .callback = handle_set_angle_command},
    { .command = 'M', .callback = handle_set_microstep_command},
    { .command = 'C', .callback = handle_set_acceleration_command},
    { .command = 'X', .callback = handle_move_by_command},
    { .command = 'T', .callback = handle_set_target_speed_command},
	{ .command = 'H', .callback = handle_help_command}
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
	handle_help_command();

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

ISR(TIMER0_COMPA_vect) {
    a4988_step(&motor1);
}