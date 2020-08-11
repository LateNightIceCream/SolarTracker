/*
 * RTC.h
 *
 *  Created on: Jul 16, 2020
 *      Author: zamza
 *  I2C for RTC DS1307
 */

#ifndef HEADERS_RTC_H_
#define HEADERS_RTC_H_

#include <msp430.h>
#include <stdint.h>
#include <headers/i2c.h>


//////////////////////////////////
/// CONSTANTS

#define RTC_SLAVE_ADDRESS 0b1101000

#define RTC_SECOND_ADDRESS  0x00
#define RTC_MINUTE_ADDRESS  0x01
#define RTC_HOUR_ADDRESS    0x02
#define RTC_DOW_ADDRESS     0x03
#define RTC_DAY_ADDRESS     0x04
#define RTC_MONTH_ADDRESS   0x05
#define RTC_YEAR_ADDRESS    0x06

#define RTC_CONTROL_ADDRESS 0x07
#define RTC_RS0             BIT0
#define RTC_RS1             BIT1
#define RTC_SQWE            BIT4


//////////////////////////////////
/// FUNCTIONS

void rtc_init();

// Enables and sets frequency on SQW pin to be 1 Hz
static int rtc_configure_squarewave_1hz(void);

// Sets the specified time/date in the RTC registers
int rtc_set_time_date( uint8_t year,
                       uint8_t month,
                       uint8_t day,
                       uint8_t hour,
                       uint8_t min,
                       uint8_t sec );


/* Reads the current date from the RTC registers
 * rx_buffer should be able to hold 7 values
 * Return format is:
 *  rx_buffer[0]: second
 *  rx_buffer[1]: minute
 *  rx_buffer[2]: hour
 *  rx_buffer[3]: day of week (not used)
 *  rx_buffer[4]: day of month
 *  rx_buffer[5]: month
 *  rx_buffer[6]: year
 */
int rtc_get_time_date(uint8_t* rx_buffer);

static uint8_t dec_to_bcd(uint8_t decimalInput);
static uint8_t bcd_to_dec(uint8_t bcdInput);


#endif /* HEADERS_RTC_H_ */
