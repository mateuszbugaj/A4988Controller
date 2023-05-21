#ifndef A4988_H
#define A4988_H

#include <stdint.h>
#include <avr/interrupt.h>
#include "../inc/gpio.h"

typedef struct {
    GPIOPin step;
    GPIOPin dir;
    uint16_t step_delay; // Delay between each step in milliseconds
} A4988;

void a4988_init(A4988* driver);
void a4988_set_direction(A4988* driver, uint8_t direction);
void a4988_step(A4988* driver);
void a4988_set_speed(A4988* driver, uint16_t speed); // Speed in steps per second

#endif // A4988_H
