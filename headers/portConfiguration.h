/*
 * port_definitions.h
 *
 *  Created on: Jul 12, 2020
 *      Author: zamza
 *
 *
 */

#ifndef PORT_DEFINITIONS_H_
#define PORT_DEFINITIONS_H_

#include <msp430.h>

void port_init(void);

//////////////////////////////////
/// Stepper limit switches

#define STEPPER_AZM_LIMIT_BIT BIT3
#define STEPPER_AZM_LIMIT_IN  P2IN
#define STEPPER_AZM_LIMIT_OUT P2OUT
#define STEPPER_AZM_LIMIT_DIR P2DIR
#define STEPPER_AZM_LIMIT_SEL P2SEL
#define STEPPER_AZM_LIMIT_IE  P2IE
#define STEPPER_AZM_LIMIT_REN P2REN

#define STEPPER_AZM_LIMIT_PUSHED (STEPPER_AZM_LIMIT_IN & STEPPER_AZM_LIMIT_BIT)

#define STEPPER_ELV_LIMIT_BIT BIT6
#define STEPPER_ELV_LIMIT_IN  P2IN
#define STEPPER_ELV_LIMIT_OUT P2OUT
#define STEPPER_ELV_LIMIT_DIR P2DIR
#define STEPPER_ELV_LIMIT_SEL P2SEL
#define STEPPER_ELV_LIMIT_IE  P2IE
#define STEPPER_ELV_LIMIT_REN P2REN

#define STEPPER_ELV_LIMIT_PUSHED (STEPPER_ELV_LIMIT_IN & STEPPER_ELV_LIMIT_BIT)

void stepper_switch_init(void);

//////////////////////////////////
/// Stepper Home Button (on board)
#define STEPPER_HOME_BUTTON_BIT BIT1
#define STEPPER_HOME_BUTTON_DIR P2DIR
#define STEPPER_HOME_BUTTON_SEL P2SEL
#define STEPPER_HOME_BUTTON_IE  P2IE
#define STEPPER_HOME_BUTTON_IES P2IES

void stepper_home_button_init(void);

//////////////////////////////////
/// I2C RTC Pins

#define I2C_DIR P4DIR
#define I2C_SEL P4SEL
#define I2C_SDA BIT1
#define I2C_SCL BIT2

void i2c_port_init(void);


//////////////////////////////////
/// RTC Time Set Pin
/// uses on-board switch

#define RTC_TIMESET_DIR P1DIR
#define RTC_TIMESET_SEL P1SEL
#define RTC_TIMESET_IE  P1IE
#define RTC_TIMESET_IES P1IES
#define RTC_TIMESET_BIT BIT1

void rtc_port_init(void);


//////////////////////////////////
/// Stepper Motors

// TA0.3 for azimuth STP Output
#define STEPPER_AZM_STP_BIT BIT4
#define STEPPER_AZM_STP_DIR P1DIR
#define STEPPER_AZM_STP_SEL P1SEL

// TA2.1 for elevation STP Output
#define STEPPER_ELV_STP_BIT BIT4
#define STEPPER_ELV_STP_DIR P2DIR
#define STEPPER_ELV_STP_SEL P2SEL

// the respective PWM timer Outputs go to the capture
// inputs of other timer units
#define STEPPER_AZM_STP_CAPTURE_BIT BIT2
#define STEPPER_AZM_STP_CAPTURE_DIR P1DIR
#define STEPPER_AZM_STP_CAPTURE_SEL P1SEL

#define STEPPER_ELV_STP_CAPTURE_BIT BIT3
#define STEPPER_ELV_STP_CAPTURE_DIR P1DIR
#define STEPPER_ELV_STP_CAPTURE_SEL P1SEL

void timer_port_init();

// DIR Bits
#define STEPPER_AZM_DIR_BIT BIT5
#define STEPPER_AZM_DIR_OUT P1OUT
#define STEPPER_AZM_DIR_DIR P1DIR
#define STEPPER_AZM_DIR_SEL P1SEL

#define STEPPER_ELV_DIR_BIT BIT5
#define STEPPER_ELV_DIR_OUT P2OUT
#define STEPPER_ELV_DIR_DIR P2DIR
#define STEPPER_ELV_DIR_SEL P2SEL

// SLEEP Bits
#define STEPPER_AZM_SLP_BIT BIT4
#define STEPPER_AZM_SLP_OUT P7OUT
#define STEPPER_AZM_SLP_DIR P7DIR
#define STEPPER_AZM_SLP_SEL P7SEL

#define STEPPER_ELV_SLP_BIT BIT2
#define STEPPER_ELV_SLP_OUT P2OUT
#define STEPPER_ELV_SLP_DIR P2DIR
#define STEPPER_ELV_SLP_SEL P2SEL

#define STEPPER_AZM_SLEEP   (STEPPER_AZM_SLP_OUT &= ~STEPPER_AZM_SLP_BIT)
#define STEPPER_AZM_WAKE    (STEPPER_AZM_SLP_OUT |=  STEPPER_AZM_SLP_BIT)

#define STEPPER_ELV_SLEEP   (STEPPER_ELV_SLP_OUT &= ~STEPPER_ELV_SLP_BIT)
#define STEPPER_ELV_WAKE    (STEPPER_ELV_SLP_OUT |=  STEPPER_ELV_SLP_BIT)

void stepper_dir_slp_init();


#endif /* PORT_DEFINITIONS_H_ */
