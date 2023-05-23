#ifndef A4988_H
#define A4988_H

#include <stdint.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "../inc/gpio.h"
#include "../inc/usart.h"

#define FORWARD 1
#define BACKWARD 0

typedef struct {
    GPIOPin step;
    GPIOPin dir;
    GPIOPin sleep;
    GPIOPin ms1;
    GPIOPin ms2;
    GPIOPin ms3;
    uint8_t microstep;
    uint16_t step_ticks; // Ticks between each step for Timer
    uint16_t current_tick;
    int32_t current_steps; // Current position of the motor in steps
    int32_t target_steps;
    uint16_t speed; // steps per second
    bool moving;
    uint8_t direction;
} A4988;

void a4988_init(A4988* driver);
void a4988_step(A4988* driver);
void a4988_set_speed(A4988* driver, uint16_t speed); // Speed in steps per second
void a4988_set_angle(A4988* driver, float angle); // Angle in degrees
float a4988_get_angle(A4988* driver); // Returns the current angle of the motor in degrees
bool a4988_is_moving(A4988* driver); // Returns true if the motor is moving
void a4988_set_resolution(A4988* driver, uint8_t microstep);

#endif // A4988_H
