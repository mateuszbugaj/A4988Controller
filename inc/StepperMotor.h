#ifndef _STEPPER_MOTOR_H_
#define _STEPPER_MOTOR_H_

#include <stdbool.h>

#include "Pin.h"

#define MAX_SPEED 4000  // 1250 // steps per second
#define STEPPER_MOTOR_SLEEP_PIN PD4

Pin stepperMotorSleep;

enum Microstepping {
    FULL,
    HALF,
    QUATER,
    EIGHT
};

enum STEPPER_MOTOR_DIRECTION {
    CLOCKWISE = 1,
    COUNTER_CLOCKWISE = -1
};

typedef struct {
    Pin* step;
    Pin* dir;
    float stepsPerMM;
    // int16_t maxPosRange; // todo: temporary solution
    enum STEPPER_MOTOR_DIRECTION motorDir; // todo: temp, it shows the motor direction/polarization, default 1 (for X axis -1)
    float position; // in MM
    int16_t toMove;
    uint8_t microstepping;
    double speed;      // steps per second
    double speedSave;  // stores speed when synchronizing motors movement
    double speedTemp;
    bool moving;
    bool notified;  // notified about stopping
    bool edgeDetected;
    bool homing;
} StepperMotor;

void stepperMotorSetMicrostepping(Pin* m1, Pin* m2, uint8_t micro);
void stepperMotorRunMotor(StepperMotor* motor);
void stepperMotorMoveTo(StepperMotor* motor, float mm);
void stepperMotorMoveBy(StepperMotor* motor, float mm);
void stepperMotorSetSpeed(StepperMotor* motor, int16_t mmPerS);

#endif /* _STEPPER_MOTOR_H_ */