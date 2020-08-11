/*
 * stepper.c
 *
 *  Created on: Jul 15, 2020
 *      Author: zamza
 */

#include <headers/obsolete/stepper_old.h>


//////////////////////////////////
/// Stepper Functions

static void stepper_init(stepper* stp) {
    stepper_set_direction(stp, 0);
    //stepper_set_step(stp, 0);
    stepper_set_sleep(stp, 0);
    stepper_set_reset(stp, 0);
}

/*
 *
 * Functions for setting the control values inside the stepper structure
 *
 * */

static void stepper_set_direction(stepper* stp, uint8_t dir) {

    if(dir <= 1) { // should only be 1 or 0
        stp->direction = dir;
        stp->controlArray[0] = dir;
    }
}

static void stepper_set_sleep(stepper* stp, uint8_t sleep) {

    if(sleep <= 1) { // should only be 1 or 0
        stp->sleep = sleep;
        stp->controlArray[2] = sleep;
    }
}

static void stepper_set_reset(stepper* stp, uint8_t reset) {

    if(reset <= 1) { // should only be 1 or 0
        stp->reset = reset;
        stp->controlArray[3] = reset;
    }
}


//////////////////////////////////
/// Stepper Control Functions

/*
 *
 * Initialize stepper control structure
 *
 * */

void stepperControl_init(stepperControl* stpCtrl, shiftRegister* sr) {

    stpCtrl->stepperAzim = malloc(sizeof(stepper));
    stpCtrl->stepperElev = malloc(sizeof(stepper));

    stpCtrl->shiftRegister = sr;

    stepper_init(stpCtrl->stepperAzim);
    stepper_init(stpCtrl->stepperElev);

}

void stepperControl_set_azimuth(stepperControl* stpCtrl) {




    stepperControl_updateShiftRegister(stpCtrl);

}

void stepperControl_set_elevation(stepperControl* stpCtrl) {

}

// move ccw until limit switch is reached
//void stepperControl_home() {
//
//}

void stepperControl_step_azimuth_cw(stepperControl* stpCtrl, uint16_t numOfSteps) {

    stpCtrl->stepperAzim->direction = 1;

    stpCtrl->stepperAzim->reset = 1;

    stpCtrl->stepperAzim->sleep = 1;

    for(int i = numOfSteps; i > 0; i--) {

        stpCtrl->stepperAzim->step = 1;
        stepperControl_updateShiftRegister(stpCtrl);
        __delay_cycles(60000);
        stpCtrl->stepperAzim->step = 0;
        stepperControl_updateShiftRegister(stpCtrl);
        __delay_cycles(60000);

    }

}

/*
 * Write control settings to shift register
 * Upper nibble is for elevation
 * Lower nibble is for azimuth
 *
 * Nibble format: Bit 0: Direction
 *                Bit 1: Step
 *                Bit 2: Sleep
 *                Bit 3: Reset
 * */

static void stepperControl_updateShiftRegister(stepperControl* stpCtrl) {

    uint8_t write_byte = 0;

    write_byte |= ( stpCtrl->stepperAzim->direction << 0 |
                    //stpCtrl->stepperAzim->step      << 1 |
                    stpCtrl->stepperAzim->sleep     << 2 |
                    stpCtrl->stepperAzim->reset     << 3 |

                    stpCtrl->stepperElev->direction << 4 |
                    //stpCtrl->stepperElev->step      << 5 |
                    stpCtrl->stepperElev->sleep     << 6 |
                    stpCtrl->stepperElev->reset     << 7    );


    shiftRegister_write(stpCtrl->shiftRegister, write_byte);

}
