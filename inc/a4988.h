#ifndef A4988_H
#define A4988_H

#include <stdint.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "hal.h"

#define FORWARD 1
#define BACKWARD 0

typedef struct {
    HALPin step;
    HALPin dir;
    HALPin sleep;
    HALPin ms1;
    HALPin ms2;
    HALPin ms3;
    uint8_t microstep;
    int32_t step_ticks; // Ticks between each step for Timer
    int32_t current_tick;
    int32_t current_steps; // Current position of the motor in steps
    int32_t target_steps;
    int32_t current_speed; // steps per second
    int32_t target_speed;
    int32_t acceleration; // steps per second squared
    float acceleration_per_tick; // acceleration divided by the number of ticks in a seccond
    float speed_increase_total; // Accumulated speed increase due to acceleration
    int32_t deceleration_point; // Number of steps needed to decelerate
    bool moving;
    int8_t direction;
	bool constantSpeed;
} A4988;

void a4988_init(A4988* driver);
void a4988_step(A4988* driver);
void a4988_set_speed(A4988* driver, uint16_t speed); // Speed in steps per second
void a4988_set_target_speed(A4988* driver, uint16_t speed); // Speed in steps per second
void a4988_set_acceleration(A4988* driver, uint16_t acceleration); // Speed in steps per second
void a4988_set_angle(A4988* driver, float angle); // Angle in degrees
float a4988_get_angle(A4988* driver); // Returns the current angle of the motor in degrees
bool a4988_is_moving(A4988* driver); // Returns true if the motor is moving
void a4988_set_microstepping(A4988* driver, uint8_t microstep);
void a4988_move_steps(A4988* const device, float steps); // Moves the motor by relative number of steps

#endif // A4988_H
