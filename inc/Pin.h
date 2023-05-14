#ifndef _PINS_H_
#define _PINS_H_

#include <avr/io.h>
#include <util/delay.h>

typedef struct {
    volatile uint8_t* PIN;
    volatile uint8_t* PORT;
    volatile uint8_t* DDR;
    uint8_t value;
} Pin;

enum State { LOW = 0,
             HIGH = 1 };

enum Dir { INPUT = 0,
           OUTPUT = 1 };

void initPin(Pin pin, enum Dir dir) ;
uint8_t readPin(Pin pin);
uint8_t writePin(Pin pin, enum State value);
void togglePin(Pin pin);
#endif  // PINS_H