#include "inc/StepperMotor.h"

void stepperMotorSetMicrostepping(Pin* m1, Pin* m2, uint8_t micro) {
    switch (micro) {
        case FULL:
            writePin(*m1, LOW);
            writePin(*m2, LOW);
            break;
        case HALF:
            writePin(*m1, HIGH);
            writePin(*m2, LOW);
            break;
        case QUATER:
            writePin(*m1, LOW);
            writePin(*m2, HIGH);
            break;
        case EIGHT:
            writePin(*m1, HIGH);
            writePin(*m2, HIGH);
            break;
        default:
            writePin(*m1, HIGH);
            writePin(*m2, LOW);
    }
}

void stepperMotorRunMotor(StepperMotor* motor) {
    if (motor->toMove == 0) {

        if (motor->moving) {
            motor->moving = false;
            motor->notified = false;
        }

        return;
    }

    if (motor->speedTemp < (MAX_SPEED / motor->speed)) {
        motor->speedTemp++;
        return;
    }

    motor->speedTemp = 0;

    if (motor->toMove > 0) {
        writePin(*motor->dir, HIGH);
        motor->toMove--;
        // motor->position--;
    } else {
        writePin(*motor->dir, LOW);
        motor->toMove++;
        // motor->position++;
    }

    writePin(*motor->step, HIGH);
    writePin(*motor->step, LOW);
}

void stepperMotorMoveTo(StepperMotor* motor, float mm) {
    motor->notified = false;

    float toMoveMM = mm - (motor->position);

    motor->toMove = toMoveMM * motor->stepsPerMM * motor->motorDir;
    motor->moving = true;
    motor->notified = true; // todo: why? shouldn't it be false?
    motor->position = mm;
    writePin(stepperMotorSleep, HIGH);
}

void stepperMotorMoveBy(StepperMotor* motor, float mm) {
    motor->notified = false;
    if (mm == 0) return;

    motor->toMove = mm * motor->stepsPerMM * motor->motorDir;
    motor->moving = true;
    motor->notified = true; // todo: why? shouldn't it be false?
    motor->position = mm;
    writePin(stepperMotorSleep, HIGH);
}

void stepperMotorSetSpeed(StepperMotor* motor, int16_t mmPerS) {
    motor->speed = mmPerS * motor->stepsPerMM;
    motor->speedSave = motor->speed;
}
