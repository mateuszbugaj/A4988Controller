#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include "../inc/USART.h"
#include "../inc/Pin.h"
#include "../inc/StepperMotor.h"

#define LED PB0
#define ANALOG_INPUT PC0
#define STEP PB1
#define DIR PB2
#define MS3 PD5
#define MS2 PD6
#define MS1 PD7
#define SLEEP PB6

/* 
    Set up 8-bit Timer/Counter0 for the stepper motor operations.

    TCCR0A
        Set CTC mode with OCRA as TOP
    TCCR0B
        Set prescaler to 8
    TIMSK0
        Enable output compare match A
    TIFR0
        Clear flags
*/
void setupTimer() {
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01);
    TIMSK0 = (1 << OCIE0A);
    TIFR0 = 0;

    OCR0A = 250;  // '1' with prescaler 64 and 8Mhz CPU it gives 0.001ms
    sei();
}

/*
    Initialize ADC
    ADMUX
        Set AREF as voltage reference)
        Left adjust the results
        Set input channel for PC0 (MUX[3:0] = 0)
    ADCSRA
        Enable ADC
        Start the first conversion
        Enable auto trigger of ADC (Probably should set this to Timer/Counter with ADCSRB)
        Enable ADC interrupt
        Set clock prescaler to 128
*/
void initAdcTemp(){
    ADMUX = (1 << ADLAR) | (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint8_t readPot(){
    return ADCH;
}

void printNumberWithLeadingZeroes(int number, uint8_t zeroes){
    if(number == 0){
        for(uint8_t i = 0; i < zeroes; i++){
            usartPrint("0");
        }
        return;
    }

    int temp = number;
    int decimals = 0;
    while(temp > 0){
        temp /= 10;
        decimals++;
    }

    for(uint8_t i = 0; i < (zeroes-decimals); i++){
        usartPrint("0");
    }
    usartPrintNum(number);
}

StepperMotor stepperMotor;
int moveBy = 20;

Pin pinMS1;
Pin pinMS2;
Pin pinMS3;
Pin pinSleep;
enum Microstepping microstepping = FULL;

bool sleeping = false;
int sleepFor = 0;

int main(void){
    clock_prescale_set(clock_div_1);
    usartInit();
    initAdcTemp();

    Pin pinStep = {&PINB, &PORTB, &DDRB, STEP};
    Pin pinDir = {&PINB, &PORTB, &DDRB, DIR};
    pinSleep = (Pin) {&PINB, &PORTB, &DDRB, SLEEP};
    pinMS1 = (Pin) {&PIND, &PORTD, &DDRD, MS1};
    pinMS2 = (Pin) {&PIND, &PORTD, &DDRD, MS2};
    pinMS3 = (Pin) {&PIND, &PORTD, &DDRD, MS3};
    initPin(pinStep, OUTPUT);
    initPin(pinDir, OUTPUT);
    initPin(pinSleep, OUTPUT);
    initPin(pinMS1, OUTPUT);
    initPin(pinMS2, OUTPUT);
    initPin(pinMS3, OUTPUT);

    stepperMotor = (StepperMotor) {&pinStep, &pinDir, 12, CLOCKWISE};
    writePin(pinSleep, HIGH);
    writePin(pinMS1, LOW);
    writePin(pinMS2, LOW);
    writePin(pinMS3, LOW);
    writePin(pinDir, HIGH);

    stepperMotorSetSpeed(&stepperMotor, 10);
    setupTimer();
    stepperMotorMoveBy(&stepperMotor, moveBy);

    DDRB |= (1 << LED);
    int sampleNumber = 0;
    uint8_t pot = 0;

    usartPrint("Start\n\r");
    while(1){
        PORTB ^= (1 << LED);
        pot = readPot();
        for(uint8_t i = 0; i < pot; i++){
            _delay_ms(1);
        }
        printNumberWithLeadingZeroes(sampleNumber++, 4);
        usartPrint(": ");
        printNumberWithLeadingZeroes(pot, 3);
        usartPrint("\n\r");
    }

    return 0;
}

ISR(TIMER0_COMPA_vect) {
    // writePin(*stepperMotor.dir, HIGH);

    // writePin(*stepperMotor.step, HIGH);
    // writePin(*stepperMotor.step, LOW);

    stepperMotorRunMotor(&stepperMotor);
    stepperMotor.toMove = 1;
    
    // stepperMotorSetSpeed(&stepperMotor, readPot());
    // if(readPot() <= 10){
    //     writePin(pinSleep, LOW);
    // } else {
    //     writePin(pinSleep, HIGH);
    // }

    // if(sleepFor > 0){
    //     sleepFor--;
    //     return;
    // }

    // if(sleepFor == 0 && sleeping == true){
    //     usartPrint("Stopped sleeping\n\r");
    //     writePin(pinSleep, HIGH);
    //     sleeping = false;
    // }

    // if(!stepperMotor.moving){
    //     switch (microstepping){
    //     case FULL:
    //         usartPrint("FULL\n\r");
    //         writePin(pinMS1, LOW);
    //         writePin(pinMS2, LOW);
    //         writePin(pinMS3, LOW);
    //         break;
    //     case HALF:
    //         usartPrint("HALF\n\r");
    //         writePin(pinMS1, HIGH);
    //         writePin(pinMS2, LOW);
    //         writePin(pinMS3, LOW);
    //         break;
    //     case QUATER:
    //         usartPrint("QUATER\n\r");
    //         writePin(pinMS1, LOW);
    //         writePin(pinMS2, HIGH);
    //         writePin(pinMS3, LOW);
    //         break;
    //     case EIGHT:
    //         usartPrint("EIGHT\n\r");
    //         writePin(pinMS1, HIGH);
    //         writePin(pinMS2, HIGH);
    //         writePin(pinMS3, LOW);
    //         break;
        
    //     default:
    //         microstepping = FULL;
    //         writePin(pinSleep, LOW);
    //         sleepFor = 1000;
    //         sleeping = true;
    //         usartPrint("Sleeping\n\r");
    //         break;
    //     }

        
    //     if(!sleeping){
    //         moveBy = moveBy * -1;
    //         stepperMotorMoveBy(&stepperMotor, moveBy);
    //         microstepping++;
    //     }
    // }
}
