
#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

#include <headers/headers.h>

int spa_update();
void interrupt_handler();

#define CALCULATION_DELTA_T_DAY_MODE       60
#define CALCULATION_DELTA_T_BEFORE_SUNRISE 1800
#define CALCULATION_DELTA_T_NIGHT_MODE     7200
#define CALCULATION_SUNRISE_MODE_FACTOR    1.1


/*
 * Stores the time and date
 *  that is returned by rtc_get_time_date()
 */
static uint8_t time_date_array[7] = {};

/*
 * SPA structure that holds the angular data
 * */
static spa_data spa;

/*
 * Handles actions after interrupt, see ISR
 * */
static uint8_t interruptAction = 0;

/*
 * Defines the second distance between spa calculations
 *
 * Is changed after sunset to go into "sleep"
 * */
static uint16_t calculation_delta_t = 15;

/*
 * M A I N
 * */
int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Global interrupt enable
    __bis_SR_register(GIE);

    /*
     * Initialization
     * */
    clock_init();
    port_init();
    i2c_init();
    rtc_init();
    stepper_init();

    /* Comment this line if you want to
     * set the date/time using button P1.1
     * (port1_isr())
     * */
    RTC_TIMESET_IE &= ~RTC_TIMESET_BIT;

    // wait for a bit to settle
    for(int i = 0; i < 100; i++) {
        _delay_cycles(60000);
    }

    /*
     * Main Loop
     * */
    while(1) {

        interrupt_handler();

        LPM3;

    }
}


/*
 * on-board push button P1.1 to set the current time and date
 *
 * */
#pragma vector=PORT1_VECTOR
__interrupt void port1_isr() {

    // check which pin caused the interrupt
    if( P1IV == 0x04 ) { // RTC_TIMESET Switch (P1.1)

        if(MOTOR_NOT_RUNNING) {

            interruptAction = 1;

            // reset IFG
            P1IFG &= ~RTC_TIMESET_BIT;

            LPM3_EXIT;

        }
    }
}


/*
 * Handles interrupts from limit switches
 *  and home button as well as second counter
 * */
#pragma vector=PORT2_VECTOR
__interrupt void port2_isr() {

    uint16_t interrupt_vector_value = P2IV;

    if(interrupt_vector_value == 0x10) { // every second

        static uint16_t second_counter = 0;

        if(MOTOR_NOT_RUNNING) {

            second_counter++;

            if(second_counter == calculation_delta_t) {

                interruptAction = 2;

                second_counter = 0;

                LPM3_EXIT;

            }
        }

    } else if(interrupt_vector_value == 0x08) { // P2.3 --> AZM Limit Switch

        azm_home_switch_isrAction();

        LPM3_EXIT;

    } else if(interrupt_vector_value == 0x0E) { // P2.6 --> ELV Limit Switch

        elv_home_switch_isrAction();

        LPM3_EXIT;

    } else if(interrupt_vector_value == 0x04) { // P2.1 --> on-board home button

        home_button_isrAction();

    }
}


/*
 * Handles the actions that happen in the main()-loop on interrupt
 *  based on value of interruptAction
 *
 * */
void interrupt_handler() {

    static uint8_t sleep_flag = 1;

    switch(interruptAction) {

        case 1: // P1.1 Switch, set time/date of RTC module

            // year, month, day, hour, minute, second
            rtc_set_time_date(20, 8, 21, 9, 51, 30);
            rtc_get_time_date(time_date_array);

            break;

        case 2: // second_counter reached calculation_delta_t

            CALC_LED_OUT |= CALC_LED_BIT;

            spa_update();
            spa_calculate(&spa);

            if(spa.zenith < (90 - ELV_SUNRISE_ANGLE)) {
                // Track mode
                if(sleep_flag == 1) {
                    home_azm();
                    home_elv();
                    calculation_delta_t = CALCULATION_DELTA_T_DAY_MODE;
                    sleep_flag = 0;
                }

                elv_set_angle(90 - spa.zenith);
                azm_set_angle(spa.azimuth - 90);

            } else {
                // Sleep mode
                home_azm();
                home_elv();

                STEPPER_AZM_SLEEP;
                STEPPER_ELV_SLEEP;

                sleep_flag = 1;
                calculation_delta_t = CALCULATION_DELTA_T_NIGHT_MODE;
            }

            CALC_LED_OUT &= ~CALC_LED_BIT;

            break;

    }

    interruptAction = 0;

}


/*
 * Initializes SPA structure
 * Has to go after rtc_init, i2c_init, etc
 *
 * */
int spa_update() {

    rtc_get_time_date(time_date_array);

    // check format of time & date
    if(
        (time_date_array[0] > 60) ||
        (time_date_array[1] > 60) ||
        (time_date_array[2] > 24) ||
        (time_date_array[4] > 31) || (time_date_array[4] == 0) ||
        (time_date_array[5] > 12) || (time_date_array[5] == 0) ||
        (time_date_array[6] > 99)
    ) return 1;

    spa.year          = 2000 + time_date_array[6];
    spa.month         = time_date_array[5];
    spa.day           = time_date_array[4];
    spa.hour          = time_date_array[2];
    spa.minute        = time_date_array[1];
    spa.second        = time_date_array[0];

    spa.timezone      = 2;
    spa.delta_ut1     = 0;
    spa.delta_t       = delta_t_polynomial(spa.year-2000);
    spa.longitude     = 11.66474;
    spa.latitude      = 54.02720;
    spa.elevation     = 10;
    spa.pressure      = 1015;
    spa.temperature   = 15;
    spa.slope         = 0;
    spa.azm_rotation  = 0; // ?
    spa.atmos_refract = 0.5667;
    spa.function      = SPA_ZA;
}
