#include "inc/Pin.h"

void initPin(Pin pin, enum Dir dir) {
    if (dir == INPUT) {
        *pin.DDR &= ~(1 << pin.value);
        *pin.PORT |= (1 << pin.value);
    } else {
        *pin.DDR |= (1 << pin.value);
        *pin.PORT &= ~(1 << pin.value);
    }
}

uint8_t readPin(Pin pin) {
    if (*pin.PIN & (1 << pin.value)) {
        _delay_ms(5);
        if (*pin.PIN & (1 << pin.value)) {
            return HIGH;
        }
    }

    return LOW;
}

uint8_t writePin(Pin pin, enum State value) {
    if (value == HIGH) {
        *pin.PORT |= (1 << pin.value);
    } else {
        *pin.PORT &= ~(1 << pin.value);
    }

    return value;
}

void togglePin(Pin pin) {
    *pin.PORT ^= (1 << pin.value);
}