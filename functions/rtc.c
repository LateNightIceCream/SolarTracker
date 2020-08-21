/*
 * RTC.c
 *
 *  Created on: Jul 23, 2020
 *      Author: zamza
 */

#include <headers/rtc.h>
#include <headers/portConfiguration.h> // needed only for rtc_init(), maybe export

static i2c_device rtc_device = {.address = RTC_SLAVE_ADDRESS};
static i2c_data   rtc_data   = {.rx_length = 0, .tx_length = 0};


/*
 *
 *
 * */

void rtc_init() {

    /*
     * Lock the MSP if there are any initialization errors
     * */

    while(rtc_configure_squarewave_1hz());

    // Enable second interrupt
    RTC_SEC_IN_IE |= RTC_SEC_IN_BIT;

}


/*
 * Converts decimal input date to bcd and writes to RTC
 * Day of week is not used
 * RTC register pointer automatically increments
 *  and is initially set to SECOND-register-address (0x00)
 *
 * */
int rtc_set_time_date( uint8_t year,
                       uint8_t month,
                       uint8_t day,
                       uint8_t hour,
                       uint8_t min,
                       uint8_t sec ) {

    int err = 1;

    if( (year  <= 99)              &&
        (month <= 12 && month > 0) &&
        (day   <= 31 && day > 0)   &&
        (hour  <= 23)              &&
        (min   <= 59)              &&
        (sec   <= 59)) {

        uint8_t bcd_sec   = dec_to_bcd(sec); // CH-Bit == 0
        uint8_t bcd_min   = dec_to_bcd(min);
        uint8_t bcd_hour  = dec_to_bcd(hour); // Bit6 == 0, 24h
        uint8_t bcd_day   = dec_to_bcd(day);
        uint8_t bcd_month = dec_to_bcd(month);
        uint8_t bcd_dow   = dec_to_bcd(0x01);
        uint8_t bcd_year  = dec_to_bcd(year);

        uint8_t tx_buffer[8] = {RTC_SECOND_ADDRESS,
                                bcd_sec,
                                bcd_min,
                                bcd_hour,
                                bcd_dow,
                                bcd_day,
                                bcd_month,
                                bcd_year};

        rtc_data.tx_buffer = tx_buffer;
        rtc_data.tx_length = 8;
        rtc_data.rx_length = 0;

        err = i2c_transfer(&rtc_device, &rtc_data);

    }

    // wait a little to settle
    for(uint8_t i = 0; i < 100; i++) {
        __delay_cycles(60000);
    }

    return err;

}


/*
 *  Sets register pointer to SECOND-register and
 *   reads the 7 time/date-bytes into rx_buffer
 *
 *  Then converts from BCD to decimal
 *
 *  rx_buffer has to be able to hold 7 values!
 *
 * */
int rtc_get_time_date(uint8_t* rx_buffer) {

    uint8_t tx_buffer[1] = {RTC_SECOND_ADDRESS};

    rtc_data.tx_buffer = tx_buffer;
    rtc_data.tx_length = 1;

    rtc_data.rx_buffer = rx_buffer;
    rtc_data.rx_length = 7;

    int err = i2c_transfer(&rtc_device, &rtc_data);

    if(err) {
        return err;
    }

    for(uint8_t i = 0; i < 7; i++) {
        rx_buffer[i] = bcd_to_dec(rx_buffer[i]);
    }

    return err;

}


/*
 * Enables and sets frequency on SQW pin to be 1 Hz
 * First creates a register configuration:
 *  - RS0 and RS1 are 0 to set frequency to 1 Hz
 *  - SQWE is 1 to enable the output
 *
 * Then writes address of control register (to set RTC
 * register pointer) followed by the register
 * configuration
 *
 * */
static int rtc_configure_squarewave_1hz(void) {

    uint8_t register_config = ( RTC_SQWE );

    uint8_t tx_buffer[2] = {RTC_CONTROL_ADDRESS, register_config};

    rtc_data.tx_buffer = tx_buffer;
    rtc_data.tx_length = 2;

    int err = i2c_transfer(&rtc_device, &rtc_data);

    return err;

}


/*
 * Manages BCD-Codes that are needed for the RTC-registers
 *  maybe speed up using enums for time/date
 * */

static uint8_t dec_to_bcd(uint8_t decimalInput) {
    return (((decimalInput / 10) << 4) | (decimalInput % 10) );
}

static uint8_t bcd_to_dec(uint8_t bcdInput) {
    return bcdInput - 6 * (bcdInput >> 4);
}


////    ;------------------------------------------------------------------------------
////    bin2bcd;     Subroutine for converting 16 bit binary to BCD
////    ;              Input: R12 is 16 bit binary
////    ;            Working: R15 is used and not saved
////    ;             Output: R14|R13 5 digit BCD
////    ;------------------------------------------------------------------------------
//
//    printf("%d\n", dst);
//
//    __asm__ goto(
//            "mov #30, R12\n\t"
//            "mov #8, R15\n\t"
//            "clr R14\n\t"
//            "clr R13\n\t"
//            "bep rla R12\n\t"
//            "dadd R13,R13\n\t"
//            "dadd R14,R14\n\t"
//            "dec R15\n\t"
//            "jnz bep\n\t"
//            "ret"
//            : /* No outputs. */
//            : /* No inputs. */
//            : /* No clobbers. */
//            : );
//
//    return 0;
//
//}
