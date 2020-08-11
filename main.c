
#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

#include <headers/headers.h>

void spa_init();
void interrupt_handler();

/*
 * Stores the time and date
 *  that is returned by rtc_get_time_date()
 */
static uint8_t time_date_array[7] = {};

/*
 * SPA structure that holds the angular data
 * */
static spa_data spa;
int result;

int returnVal = 0;

/*
 * Keeps track of current position on both axes
 */
static double azm_currentAngle = AZM_HOME_SWITCH_ANGLE;
static double elv_currentAngle = ELV_HOME_SWITCH_ANGLE;

/*
 * Handles actions after interrupt, see ISR
 * */
static uint8_t interruptAction = 0;

//////////////////////////////////
/// M A I N

int main(void)
{
    // Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;
	
	// Global interrupt enable
	__bis_SR_register(GIE);


	//////////////////////////////////
	/// Initializations

	clock_init();
	port_init();
    //i2c_init();
    //rtc_init();
    stepper_init();
    spa_init();

    for(int i = 0; i < 100; i++) {
        _delay_cycles(60000); // wait to settle
    }

    home_azm();

    azm_set_angle(180);

    azm_set_angle(90);

    azm_set_angle((180+90)/2);


    //////////////////////////////////
    /// Main Loop

	while(1) {

	    LPM3;

	    interrupt_handler();

	}
}


//////////////////////////////////
/// Limit switch interrupts

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

    if(1) {


    } else if(1) {


    }
}


/*
 * Handles the actions that happen in the main()-loop on interrupt
 *  based on value of interruptAction
 *
 * */

void interrupt_handler() {

    switch(interruptAction) {

        case 1: // P1.1 Switch, set time/date of RTC module

            //rtc_set_time_date(20, 7, 23, 21, 50, 20);
            //rtc_get_time_date(time_date_array);

            break;

        case 2: // P?.? Switch, go to home position
            break;

    }

    interruptAction = 0;

}


/*
 * Initializes SPA structure
 * Has to go after rtc_init, i2c_init, etc
 *
 * */
void spa_init() {
   // rtc_get_time_date(time_date_array);

    spa.year          = 2000 + 20; // time_date_array[6]
//    spa.month         = time_date_array[5];
//    spa.day           = time_date_array[4];
//    spa.hour          = time_date_array[2];
//    spa.minute        = time_date_array[1];
//    spa.second        = ;
    spa.year = 2020;
    spa.month = 7;
    spa.day = 27;
    spa.hour = 18;
    spa.minute = 5;
    spa.second = 20;
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
