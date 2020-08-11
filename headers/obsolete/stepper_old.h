/*
 * stepper.h
 *
 *  Created on: Jul 12, 2020
 *      Author: zamza
 */

#ifndef HEADERS_STEPPER_H_
#define HEADERS_STEPPER_H_

#include <headers/shiftRegister.h>
#include <stdint.h>
#include <stdlib.h>

#define STEPS_PER_REVOLUTION 6400 // 1/32 uStepping
#define STEPS_PER_DEGREE     17.78

typedef struct stepper_ {

    float angle;

    // control signals, 1 or 0
    uint8_t direction;
    uint8_t step;
    uint8_t sleep;
    uint8_t reset;

    uint8_t controlArray[4];

} stepper;

typedef struct stepperControl_ {

    stepper* stepperAzim;
    stepper* stepperElev;

    shiftRegister* shiftRegister;

} stepperControl;

void stepperControl_init(stepperControl* stpCtrl, shiftRegister* sr);

void stepperControl_set_azimuth(stepperControl* stpCtrl);

void stepperControl_set_elevation(stepperControl* stpCtrl);
void stepperControl_step_azimuth_cw(stepperControl* stpCtrl, uint16_t numOfSteps);

static void stepper_updateControlArray(stepperControl* stpCtrl);
static void stepperControl_updateShiftRegister(stepperControl* stpCtrl);

static void stepper_set_direction(stepper* stp, uint8_t dir);
static void stepper_set_step(stepper* stp, uint8_t step);
static void stepper_set_sleep(stepper* stp, uint8_t sleep);
static void stepper_set_reset(stepper* stp, uint8_t reset);

#endif /* HEADERS_STEPPER_H_ */
