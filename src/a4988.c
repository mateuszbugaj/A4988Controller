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
    gpio_pin_write(driver->sleep, LOW);
    gpio_pin_write(driver->ms1, LOW);
    gpio_pin_write(driver->ms2, LOW);
    gpio_pin_write(driver->ms3, LOW);
    driver->step_ticks = 0;
    driver->microstep = 1;

    // Set default speed
    a4988_set_speed(driver, 20); // Default to 20 steps per second

    timer1_init();
}

void a4988_set_resolution(A4988* driver, uint8_t microstep) {
    driver->microstep = microstep;

    switch (microstep) {
        case 1:  // Full step
            usart_print("Full step\n\r");
            gpio_pin_write(driver->ms1, LOW);
            gpio_pin_write(driver->ms2, LOW);
            gpio_pin_write(driver->ms3, LOW);
            break;

        case 2:  // Half step
            usart_print("Half step\n\r");
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
        if(driver->current_tick != driver->step_ticks / driver->microstep){
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
            gpio_pin_write(driver->sleep, LOW);
        }
    }
}

void a4988_set_speed(A4988* driver, uint16_t speed) {
    usart_print("Set speed : ");
    usart_print_num(speed);
    usart_print("\n\r");

    if (speed > 0) {
        driver->speed = speed;
        // Convert speed in steps per second to ticks per step
        driver->current_tick = 0;
        driver->step_ticks = TICKS_PER_SECOND / speed;
    }
}

void a4988_set_angle(A4988* driver, float angle) {
    usart_print("Set angle : ");
    usart_print_num(angle);
    usart_print("\n\r");

    // Calculate the number of steps for the desired angle
    int32_t desired_steps = (int32_t)(angle / 360.0 * STEPS_PER_REVOLUTION * driver->microstep);
    
    // Set the target steps and adjust the direction of movement
    if(driver->current_steps == desired_steps) return;

    driver->direction = driver->current_steps < desired_steps ? FORWARD : BACKWARD;
    driver->moving = true;
    gpio_pin_write(driver->sleep, HIGH);
    driver->target_steps = desired_steps;
}

float a4988_get_angle(A4988* driver) {
    // Convert the current position in steps to degrees
    return (float)(360.0 / STEPS_PER_REVOLUTION * driver->current_steps);
}

bool a4988_is_moving(A4988* driver) {
    return driver->moving;
}
