#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>

typedef enum {
    INPUT = 0,
    OUTPUT = 1
} PinDirection;

typedef enum {
    LOW = 0,
    HIGH = 1
} PinLevel;

typedef struct {
    volatile uint8_t* port;
    uint8_t pin;
} GPIOPin;

void gpio_pin_direction(GPIOPin pin, PinDirection direction);
void gpio_pin_write(GPIOPin pin, PinLevel level);
PinLevel gpio_pin_read(GPIOPin pin);

#endif // GPIO_H
