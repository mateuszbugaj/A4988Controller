#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>

#include "../inc/gpio.h"
#include "../inc/a4988.h"
#include "../inc/parser.h"

#define DELAY_MS 500
#define NUM_STEPS 200

// Define the A4988 motor1 pins globally
A4988 motor1 = {
    .step = { .port = &PORTB, .pin = PB1 },
    .dir = { .port = &PORTB, .pin = PB2 },
    .sleep = { .port = &PORTB, .pin = PB6},
    .ms1 = { .port = &PORTD, .pin = PB6},
    .ms2 = { .port = &PORTD, .pin = PD5},
    .ms3 = { .port = &PORTB, .pin = PB7}
};

GPIOPin pinLED = { .port = &PORTB, .pin = PB0 };

void handle_set_speed_command(uint16_t speed) {
    a4988_set_speed(&motor1, speed);
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
    { .command = 'M', .callback = handle_set_microstep_command}
};

bool x = true;

int main(void) {
    clock_prescale_set(clock_div_1);

    // Initialize A4988 motor1
    a4988_init(&motor1);

    // Initialize USART
    usart_init();
    parser_init(command_handlers, sizeof(command_handlers)/sizeof(command_handlers[0]));

    gpio_pin_direction(pinLED, OUTPUT);
    usart_print("Start\n\r");

    a4988_set_speed(&motor1, 2);
    // a4988_set_angle(&motor1, 45);
    a4988_set_microstepping(&motor1, 1);
    while (1) {
        if(a4988_is_moving(&motor1)){
            gpio_pin_write(pinLED, HIGH);
        } else {
            gpio_pin_write(pinLED, LOW);

            // _delay_ms(1000);
            
            // if(x){
            //     a4988_set_angle(&motor1, 180);
            //     x = false;
            // } else {
            //     a4988_set_angle(&motor1, 0);
            //     x = true;
            // }
        }
    }

    return 0;
}

ISR(TIMER1_COMPA_vect) {
    a4988_step(&motor1);
}