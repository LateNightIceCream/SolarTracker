/*
 * stepper.h
 *
 *  Created on: Jul 28, 2020
 *      Author: zamza
 */

#ifndef HEADERS_STEPPER_H_
#define HEADERS_STEPPER_H_

#include <stdint.h>
#include <math.h>
#include <msp430.h>

// (1.8deg with 1/32 Microstepping )
#define DEGREES_PER_STEP 0.05625

// check if motors are currently moving (Timers running)
#define MOTOR_NOT_RUNNING (((TA0CTL & MC__CONTINUOUS) == MC__STOP) && ((TA2CTL & MC__CONTINUOUS) == MC__STOP))
#define MOTOR_IS_RUNNING  !MOTOR_NOT_RUNNING

#define AZM_SET_DIRECTION_HOME STEPPER_AZM_DIR_OUT |=  STEPPER_AZM_DIR_BIT
#define AZM_SET_DIRECTION_SPA  STEPPER_AZM_DIR_OUT &= ~STEPPER_AZM_DIR_BIT

#define ELV_SET_DIRECTION_HOME STEPPER_ELV_DIR_OUT &= ~STEPPER_ELV_DIR_BIT
#define ELV_SET_DIRECTION_SPA  STEPPER_ELV_DIR_OUT |=  STEPPER_ELV_DIR_BIT

#define AZM_HOME_SWITCH_ANGLE 0.0
#define ELV_HOME_SWITCH_ANGLE -47

#define AZM_ANGLE_LIMIT 280
#define ELV_ANGLE_LIMIT 360.0

#define ELV_SUNRISE_ANGLE -6
#define ELV_SUNSET_ANGLE  -6

void stepper_init();

static void timer_a0_init();
int turn_degrees_azm(double degrees);
int turn_degrees_elv(double degrees);

int azm_set_angle(double degrees);
int elv_set_angle(double degrees);

int home_azm(void);
int home_elv(void);

void azm_home_switch_isrAction();
void elv_home_switch_isrAction();
void home_button_isrAction();

#endif /* HEADERS_STEPPER_H_ */
