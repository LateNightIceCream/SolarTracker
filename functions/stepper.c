/*
 * stepper.c
 *
 *  Created on: Jul 28, 2020
 *      Author: zamza
 *
 *
 *  The stepper driver DRV8825 has 4 relevant pins:
 *      - STEP
 *      - DIR
 *      - ~RESET
 *      - ~SLEEP
 *
 *  STEP is driven by a PWM Signal from
 *     - TIMERA0.3 for azimuth
 *     - TIMERA2.1 for elevation
 *
 *  DIR is driven by regular I/O
 *  ~SLEEP is driven by regular I/O
 *  ~RESET is not used, pulled high
 *
 *
 */

#include <headers/stepper.h>
#include <headers/portConfiguration.h>

/*
 *
 * The amount of steps to step
 *  / The timer CCRx Value
 *  is decremented until == 0
 *
 * */
static uint16_t azm_target_number_of_edges = 0;
static uint16_t elv_target_number_of_edges = 0;

// tracks current angle
static double azm_current_angle = 0;
static double elv_current_angle = 0;

extern uint8_t interruptAction;

void stepper_init() {

    timer_a0_init();

    STEPPER_AZM_WAKE;
    STEPPER_ELV_WAKE;

}


/*
 * Initializes TimerA0 to create a PWM Signal to drive STEP
 *  and Capture AZM and ELV STEP Signals
 *
 * */
static void timer_a0_init() {

    // select SMCLK, input divider 1, clear div. logic
    TA0CTL |= TASSEL__SMCLK | ID__1 | TACLR;
    TA2CTL |= TASSEL__SMCLK | ID__1 | TACLR;

    /// TA0.1 Capture AZM Step PWM Signal from CCR3
        // Capture on RE, select CCI1A, Capture Mode, enable Interrupts
        TA0CCTL1 |= CM_1 | CCIS_0 | CAP | CCIE;

    /// AZM PWM:
        // 50% duty cycle
        TA0CCR3 = 32768;

        // CCR2, OUTMOD7
        TA0CCTL3 |= OUTMOD_7;


    /// TA0.2 Capture ELV Step PWM Signal fron CCR4
        // Capture on RE, select CCI1A, Capture Mode, enable Interrupts
        TA0CCTL2 |= CM_1 | CCIS_0 | CAP | CCIE;

    /// ELV PWM:
        // 50% duty cycle
        TA2CCR1 = 32768;

        // CCR1 OUTMOD7
        TA2CCTL1 |= OUTMOD_7;

}


/*
 * Turns the specified amount of degrees on azimuth axis
 *  if amount is within the resolution and angular limits
 *  of the stepper
 *
 * Calculates (FLOP) the number of steps needed for degrees
 *  and starts the timer unit TA2(.1)
 *
 * degrees < 0
 *  DIR == HIGH, rotating towards home position
 *
 * degrees > 0
 *  DIR == LOW, rotating away from home position
 *
 *  works together with TA0ISR (capture)
 *
 *  if multiple calls are done, i.e. the function is called
 *   when there is currently a motor running, the action
 *   is buffered by putting the controller into LPM3 and
 *   exiting on capture interrupt
 *
 * */
int turn_degrees_azm(double degrees) {

    if(degrees < 0 && STEPPER_AZM_LIMIT_PUSHED) {
        return 2;
    }

    if(MOTOR_IS_RUNNING) {
        LPM3;
    }

    if(degrees < 0)  {
        AZM_SET_DIRECTION_HOME;
    } else {

        if(STEPPER_AZM_LIMIT_PUSHED) {
            STEPPER_AZM_WAKE;
        }

        AZM_SET_DIRECTION_SPA;
    }

    // calculate number of pulses needed to turn the
    // desired amount of degrees
    azm_target_number_of_edges = fabs(degrees) / DEGREES_PER_STEP;

    if(azm_target_number_of_edges == 0) {
        return 2;
    }

    // start continuous mode
    TA0CTL |= MC__CONTINUOUS;

    LPM3;

    return 0;
}


/*
 * Turns the specified amount of degrees on elevation axis
 *  if amount is within the resolution and angular limits
 *  of the stepper
 *
 * Calculates (FLOP) the number of steps needed for degrees
 *  and starts the timer unit TA2(.1)
 *
 * degrees < 0
 *  DIR == HIGH, rotating towards home position
 *
 * degrees > 0
 *  DIR == LOW, rotating away from home position
 *
 *  works together with TA0ISR (capture)
 *
 *  if multiple calls are done, i.e. the function is called
 *   when there is currently a motor running, the action
 *   is buffered by putting the controller into LPM3 and
 *   exiting on capture interrupt
 *
 * */
int turn_degrees_elv(double degrees) {

    if(degrees < 0 && STEPPER_ELV_LIMIT_PUSHED) {
        return 1;
    }

    if(MOTOR_IS_RUNNING) {
        LPM3;
    }

    if(degrees < 0)  {
        ELV_SET_DIRECTION_HOME;
    } else {

        if(STEPPER_AZM_LIMIT_PUSHED) {
            STEPPER_AZM_WAKE;
        }

        ELV_SET_DIRECTION_SPA;

    }

    // calculate number of pulses needed to turn the
    // desired amount of degrees
    elv_target_number_of_edges = fabs(degrees) / DEGREES_PER_STEP;

    if(elv_target_number_of_edges == 0) {
        return 2;
    }

    // start continuous mode
    TA2CTL |= MC__CONTINUOUS;

    LPM3;

    return 0;
}


/*
 * Sets the specified angle on the azimuth plane
 * Starts at
 * */
int azm_set_angle(double degrees) {

    if(degrees > AZM_ANGLE_LIMIT) {
        return 1;
    } else if(degrees < AZM_HOME_SWITCH_ANGLE) {
        return 2;
    }

    if(turn_degrees_azm(degrees-azm_current_angle) == 2) {
        return 3;
    }

    azm_current_angle = degrees;

    return 0;

}


/*
 * Sets the current angle on the elevation plane
 * Starts
 * */
int elv_set_angle(double degrees) {

    if(degrees > ELV_ANGLE_LIMIT) {
        return 1;
    } else if(degrees < ELV_HOME_SWITCH_ANGLE) {
        return 2;
    }

    if(turn_degrees_elv(degrees-elv_current_angle) == 2) {
        return 3;
    }

    elv_current_angle = degrees;

    return 0;

}


/*
 * Turns motor in home direction
 *  until Limit Switch Interrupt triggers
 * */
int home_azm() {

    if(STEPPER_AZM_LIMIT_PUSHED) {
        return 1;
    }

    if(MOTOR_IS_RUNNING) {
        return 2;
    }

    STEPPER_AZM_WAKE;

    // home direction
    AZM_SET_DIRECTION_HOME;

    // start continuous mode
    TA0CTL |= MC__CONTINUOUS;

    LPM3;

    azm_current_angle = AZM_HOME_SWITCH_ANGLE;

    return 0;
}


/*
 * Turns motor in home direction
 *  until Limit Switch Interrupt triggers
 * */
int home_elv() {

    if(STEPPER_ELV_LIMIT_PUSHED) {
        return 1;
    }

    if(MOTOR_IS_RUNNING) {
        return 2;
    }

    STEPPER_ELV_WAKE;

    // home direction
    ELV_SET_DIRECTION_HOME;

    // start continuous mode
    TA2CTL |= MC__CONTINUOUS;

    LPM3;

    return 0;
}


/*
 * To be placed inside the PORT2 ISR
 * */

void azm_home_switch_isrAction() {

    TA0CTL &= ~MC__CONTINUOUS;

    P2IFG &= ~STEPPER_AZM_LIMIT_BIT;

    // just for safety
    P2IFG &= ~STEPPER_HOME_BUTTON_BIT;

    azm_current_angle = AZM_HOME_SWITCH_ANGLE;

    //azm_target_number_of_edges = 0;

}

void elv_home_switch_isrAction() {

    TA2CTL &= ~MC__CONTINUOUS;

    P2IFG &= ~STEPPER_ELV_LIMIT_BIT;

    // just for safety
    P2IFG &= ~STEPPER_HOME_BUTTON_BIT;

    elv_current_angle = ELV_HOME_SWITCH_ANGLE;

    //elv_target_number_of_edges = 0;

}

void home_button_isrAction() {

    home_elv();
    home_azm();

    P2IFG &= ~STEPPER_HOME_BUTTON_BIT;
}


/*
 * Handles interrupts from capture compare feed back
 * TA0.1 is for azm PWM
 * Ta0.2 is for elv PWM
 *
 * */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void timera0_isr() {

    // has to save into variable because it TAxIV is reset on read
    uint16_t interrupt_vector_value = TA0IV;

    if (interrupt_vector_value & 0x02) { // Interrupt source: TA0.1 CCIFG

        if(--azm_target_number_of_edges == 0) {

            // turn off Timer A0 to stop rotation
            TA0CTL &= ~MC_3;

            LPM3_EXIT;

        }

    } else if (interrupt_vector_value & 0x04) { // Interrupt source: TA0.2 CCIFG

        if(--elv_target_number_of_edges == 0) {

            // turn off Timer A2 to stop rotation
            TA2CTL &= ~MC_3;

            LPM3_EXIT;

        }
    }
}
