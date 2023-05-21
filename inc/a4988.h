#ifndef A4988_H
#define A4988_H

#include <stdint.h>
#include "../inc/gpio.h"

typedef struct {
    GPIOPin step;
    GPIOPin dir;
} A4988;

void a4988_init(A4988* driver);
void a4988_set_direction(A4988* driver, uint8_t direction);
void a4988_step(A4988* driver);

#endif // A4988_H
