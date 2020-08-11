/*
 * i2c.h
 *
 *  Created on: Jul 17, 2020
 *      Author: zamza
 *
 *  i2c functionality (for RTC chip)
 *  using USCIB0
 *
 */

#ifndef HEADERS_I2C_H_
#define HEADERS_I2C_H_

#include <headers/clockConfiguration.h>
#include <msp430.h>
#include <stdlib.h>

// initialize USCI to I2C
void i2c_init(void);

// Structures that define the i2c device and data
typedef struct i2c_device_ {

    uint8_t address;

} i2c_device;

typedef struct i2c_data_ {

    const void* tx_buffer;
    size_t      tx_length;
    void*       rx_buffer;
    size_t      rx_length;

} i2c_data;


// Function handling data transfer:
// Master transmit, slave receive
// Master receive, slave transmit
// and combined
int i2c_transfer(const i2c_device* device, const i2c_data* data);

// Function for transmitting a buffer of bytes over I2C
static int transmit(const i2c_device* device, const uint8_t* buffer, size_t numberOfBytes);

// Function for receiving a buffer of bytes over I2C
static int receive(const i2c_device* device, uint8_t* buffer, size_t numberOfBytes);

// Function for checking N/ACK, returns -1 if NACK
static int check_acknowledge(const i2c_device* device);

#endif /* HEADERS_I2C_H_ */
