#include <util/delay.h>
#include "../inc/a4988.h"

#define CLOCK_FREQUENCY 8000000
#define PRESCALER 256
#define STEPS_PER_REVOLUTION 200
#define TICKS_PER_SECOND 1000

void timer1_init(void) {
    // Set up timer with prescaler = 64 and CTC mode
    TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);
    TCCR1A = 0;
    TCNT1 = 0;

    /* 
        Initialize compare value
        (F_CPU / Prescaler / Desired Interrupt Frequency) - 1. 
    */
    OCR1A = 124; 

    // Enable global interrupts
    sei();
}


void a4988_init(A4988* driver) {
    gpio_pin_direction(driver->dir, OUTPUT);
    gpio_pin_direction(driver->step, OUTPUT);
    gpio_pin_direction(driver->sleep, OUTPUT);
    gpio_pin_direction(driver->ms1, OUTPUT);
    gpio_pin_direction(driver->ms2, OUTPUT);
    gpio_pin_direction(driver->ms3, OUTPUT);
    gpio_pin_write(driver->sleep, HIGH);
    gpio_pin_write(driver->ms1, LOW);
    gpio_pin_write(driver->ms2, LOW);
    gpio_pin_write(driver->ms3, LOW);
    driver->step_ticks = 0;
    driver->speed_increase_total = 0;
    driver->microstep = 1;
	driver->constantSpeed = false;

    // Set default speed
    // a4988_set_speed(driver, 20); // Default to 20 steps per second
    a4988_set_acceleration(driver, 1);
    a4988_set_target_speed(driver, 20);

    timer1_init();
}

void a4988_set_microstepping(A4988* driver, uint8_t microstep) {
    if(microstep == driver->microstep) return;

    float microstepFloat = microstep;
    float microstepScaleFactor = (float)(microstepFloat / driver->microstep);

    driver->microstep = microstep;
    driver->target_steps *= microstepScaleFactor;
    driver->current_steps *= microstepScaleFactor;
    driver->current_tick = 0;

    switch (microstep) {
        case 1:  // Full step
            gpio_pin_write(driver->ms1, LOW);
            gpio_pin_write(driver->ms2, LOW);
            gpio_pin_write(driver->ms3, LOW);
            break;

        case 2:  // Half step
            gpio_pin_write(driver->ms1, HIGH);
            gpio_pin_write(driver->ms2, LOW);
            gpio_pin_write(driver->ms3, LOW);
            break;

        case 4:  // Quarter step
            gpio_pin_write(driver->ms1, LOW);
            gpio_pin_write(driver->ms2, HIGH);
            gpio_pin_write(driver->ms3, LOW);
            break;

        case 8:  // Eighth step
            gpio_pin_write(driver->ms1, HIGH);
            gpio_pin_write(driver->ms2, HIGH);
            gpio_pin_write(driver->ms3, LOW);
            break;

        default:
            // Invalid resolution, default to full step
            gpio_pin_write(driver->ms1, LOW);
            gpio_pin_write(driver->ms2, LOW);
            gpio_pin_write(driver->ms3, LOW);
            break;
    }
}

void a4988_step(A4988* driver) {
    if (driver->moving) {
		if(driver->constantSpeed == false){
			int32_t remaining_steps = abs(driver->target_steps - driver->current_steps);

			// Accelerate if not reached the target speed or decalaretion point
			if(driver->current_speed < driver->target_speed && remaining_steps > driver->deceleration_point){
				driver->speed_increase_total += driver->acceleration_per_tick;
			} else if(remaining_steps < driver->deceleration_point && driver->current_speed > 5){
				driver->speed_increase_total -= driver->acceleration_per_tick;
			}

			int8_t speed_increase = (int8_t)driver->speed_increase_total;
			driver->current_speed += speed_increase;
			driver->speed_increase_total -= (float)speed_increase;

			// Update step_ticks according to the new speed
			if(driver->current_speed != 0){
				driver->step_ticks = TICKS_PER_SECOND / driver->current_speed;
			} else {
				return;
			}
		}

        if(driver->current_tick < driver->step_ticks / driver->microstep){
            driver->current_tick++;
            return;
        } else {
            driver->current_tick = 0;
        }

        if(driver->direction == FORWARD){
            gpio_pin_write(driver->dir, HIGH);
        } else {
            gpio_pin_write(driver->dir, LOW);
        }

        gpio_pin_write(driver->step, HIGH);
        _delay_us(5); // Signal rise time
        gpio_pin_write(driver->step, LOW);

        driver->current_steps += driver->direction == FORWARD ? 1 : -1;
        if (driver->current_steps == driver->target_steps) {
            driver->moving = false;
            driver->current_speed = 0;
        }
    }
}

void a4988_set_target_speed(A4988* driver, uint16_t speed) {
	driver->constantSpeed = false;
	driver->target_speed = speed;
	driver->current_speed = 0;
	driver->speed_increase_total = 0;
}

void a4988_set_speed(A4988* driver, uint16_t speed) {
	driver->constantSpeed = true;
	driver->current_speed = speed;
	driver->speed_increase_total = 0;
	// Convert speed in steps per second to ticks per step
	driver->current_tick = 0;
	driver->step_ticks = TICKS_PER_SECOND / speed;
}

void a4988_set_acceleration(A4988* driver, uint16_t acceleration){
	driver->constantSpeed = false;
	driver->acceleration = acceleration;
	driver->acceleration_per_tick = (float)((float)acceleration/TICKS_PER_SECOND);
	driver->speed_increase_total = 0;
}

void a4988_set_angle(A4988* driver, float angle) {    
    int32_t target_steps = (int32_t)(angle / 360.0 * STEPS_PER_REVOLUTION * driver->microstep); // Number of steps for the desired angle
    if(driver->current_steps == target_steps) return;

    int64_t distance_to_target_speed = ((driver->target_speed * driver->target_speed) / (2.0 * driver->acceleration)) * driver->microstep;
    int32_t abs_steps_difference = abs(target_steps - driver->current_steps); // The absolute difference between the current and desired steps

    if(distance_to_target_speed > abs_steps_difference/2){
        driver->deceleration_point = abs_steps_difference/2;
    } else {
        driver->deceleration_point = distance_to_target_speed;
    }

    driver->target_steps = target_steps;
    driver->direction = driver->current_steps < target_steps ? FORWARD : BACKWARD;
    driver->moving = true;
}

float a4988_get_angle(A4988* driver) {
    // Convert the current position in steps to degrees
    return (float)(360.0 / STEPS_PER_REVOLUTION * driver->current_steps);
}

bool a4988_is_moving(A4988* driver) {
    return driver->moving;
}

void a4988_set_steps(A4988* driver, int64_t target_steps) {    
    if(driver->current_steps == target_steps) return;

    int64_t distance_to_target_speed = ((driver->target_speed * driver->target_speed) / (2.0 * driver->acceleration)) * driver->microstep;
    int32_t abs_steps_difference = abs(target_steps - driver->current_steps); // The absolute difference between the current and desired steps

    if(distance_to_target_speed > abs_steps_difference/2){
        driver->deceleration_point = abs_steps_difference/2;
    } else {
        driver->deceleration_point = distance_to_target_speed;
    }

    driver->target_steps = target_steps;
    driver->direction = driver->current_steps < target_steps ? FORWARD : BACKWARD;
    driver->moving = true;
}


void a4988_move_steps(A4988* const device, float steps) {
    int32_t steps_to_take = steps * device->microstep;
    int32_t new_position = device->current_steps + steps_to_take;
    a4988_set_steps(device, new_position);
}