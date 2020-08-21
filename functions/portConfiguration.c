/*
 * port_configuration.c
 *
 *  Created on: Jul 16, 2020
 *      Author: zamza
 */

#include <headers/portConfiguration.h>

void port_init(void) {

    stepper_switch_init();

    stepper_dir_slp_init();

    stepper_home_button_init();

    i2c_port_init();

    rtc_port_init();

    timer_port_init();

    led_init();

}

void stepper_switch_init() {

   // Input-direction:
   STEPPER_AZM_LIMIT_DIR &= ~STEPPER_AZM_LIMIT_BIT;
   STEPPER_ELV_LIMIT_DIR &= ~STEPPER_ELV_LIMIT_BIT;

   // Select I/O:
   STEPPER_AZM_LIMIT_SEL &= ~STEPPER_AZM_LIMIT_BIT;
   STEPPER_ELV_LIMIT_SEL &= ~STEPPER_ELV_LIMIT_BIT;

   // Enable Resistors
   STEPPER_AZM_LIMIT_REN |= STEPPER_AZM_LIMIT_BIT;
   STEPPER_ELV_LIMIT_REN |= STEPPER_ELV_LIMIT_BIT;
   // Pull-Down
   STEPPER_AZM_LIMIT_OUT &= ~STEPPER_AZM_LIMIT_BIT;
   STEPPER_ELV_LIMIT_OUT &= ~STEPPER_ELV_LIMIT_BIT;

   // Enable port interrupts
   STEPPER_AZM_LIMIT_IE |= STEPPER_AZM_LIMIT_BIT;
   STEPPER_ELV_LIMIT_IE |= STEPPER_ELV_LIMIT_BIT;

}

void i2c_port_init() {

    // Output-direction for SDA and SCL:
    I2C_DIR |= (I2C_SDA | I2C_SCL);

    // Select secondary function (I2C):
    I2C_SEL |= (I2C_SDA | I2C_SCL);

}

void rtc_port_init() {

    /// TimeSet-Button
    // Input-direction:
    RTC_TIMESET_DIR &= ~RTC_TIMESET_BIT;

    // Select I/O:
    RTC_TIMESET_SEL &= ~RTC_TIMESET_BIT;

    // Enable resistor
    P1REN |= RTC_TIMESET_BIT;

    // Set resistor to pull-up
    P1OUT |= RTC_TIMESET_BIT;

    // Edge-Select: HIGH-->LOW:
    RTC_TIMESET_IES |= RTC_TIMESET_BIT;

    // Enable Port-Interrupts
    RTC_TIMESET_IE  |= RTC_TIMESET_BIT;

    /// SecondInput
    // Input-Direction
    RTC_SEC_IN_DIR &= ~RTC_SEC_IN_BIT;

    // Select I/O
    RTC_SEC_IN_SEL &= ~RTC_SEC_IN_BIT;

    // Enable Resistor
    RTC_SEC_IN_REN |= RTC_SEC_IN_BIT;

    // Set resistor to pull-down
    RTC_SEC_IN_OUT &= ~RTC_SEC_IN_BIT;

    // Interrupts are enabled in rtc_init()

}

void timer_port_init() {

    /// STEP Bits for PWM Output for stepper motors
    // Output Direction:
    STEPPER_AZM_STP_DIR |= STEPPER_AZM_STP_BIT;
    STEPPER_ELV_STP_DIR |= STEPPER_ELV_STP_BIT;

    // Select Timer function:
    STEPPER_AZM_STP_SEL |= STEPPER_AZM_STP_BIT;
    STEPPER_ELV_STP_SEL |= STEPPER_ELV_STP_BIT;

    /// Feed AZM Output back into TA0.1
    /// Feed ELV Output back into TA0.2
    // Input Direction:
    STEPPER_AZM_STP_CAPTURE_DIR &= ~STEPPER_AZM_STP_CAPTURE_BIT;
    STEPPER_ELV_STP_CAPTURE_DIR &= ~STEPPER_ELV_STP_CAPTURE_BIT;

    // Select Timer function:
    STEPPER_AZM_STP_CAPTURE_SEL |= STEPPER_AZM_STP_CAPTURE_BIT;
    STEPPER_ELV_STP_CAPTURE_SEL |= STEPPER_ELV_STP_CAPTURE_BIT;

}

void stepper_dir_slp_init() {

    /// DIR Pins, Output direction, select I/O
        // AZM
        STEPPER_AZM_DIR_DIR |=  STEPPER_AZM_DIR_BIT;
        STEPPER_AZM_DIR_SEL &= ~STEPPER_AZM_DIR_BIT;

        // ELV
        STEPPER_ELV_DIR_DIR |=  STEPPER_ELV_DIR_BIT;
        STEPPER_ELV_DIR_SEL &= ~STEPPER_ELV_DIR_BIT;

    /// ~SLEEP Pins, Output direction, select I/O, init HIGH
        // AZM
        STEPPER_AZM_SLP_DIR |=  STEPPER_AZM_SLP_BIT;
        STEPPER_AZM_SLP_SEL &= ~STEPPER_AZM_SLP_BIT;

        // sleep stepper
        STEPPER_AZM_SLP_OUT |= STEPPER_AZM_SLP_BIT;

        // ELV
        STEPPER_ELV_SLP_DIR |=  STEPPER_ELV_SLP_BIT;
        STEPPER_ELV_SLP_SEL &= ~STEPPER_ELV_SLP_BIT;

        // sleep stepper
        STEPPER_ELV_SLP_OUT |= STEPPER_ELV_SLP_BIT;

}

void stepper_home_button_init() {

    // Input-direction:
    STEPPER_HOME_BUTTON_DIR &= ~STEPPER_HOME_BUTTON_BIT;

    // Select I/O:
    STEPPER_HOME_BUTTON_SEL &= ~STEPPER_HOME_BUTTON_BIT;

    // Edge-Select: HIGH-->LOW:
    STEPPER_HOME_BUTTON_IES |= STEPPER_HOME_BUTTON_BIT;

    // Enable Port-Interrupts
    STEPPER_HOME_BUTTON_IE  |= STEPPER_HOME_BUTTON_BIT;


}

void led_init() {
    CALC_LED_DIR |=  CALC_LED_BIT;
    CALC_LED_OUT &= ~CALC_LED_BIT;
}

