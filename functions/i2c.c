/*
 * i2c.c
 *
 *  Created on: Jul 17, 2020
 *      Author: zamza
 */


#include <headers/i2c.h>

void i2c_init() {

    // Put USCI into software reset for configuration
    UCB1CTL1 |= UCSWRST;

    // Synchronous mode, I2C mode, Master mode
    UCB1CTL0 |= (UCSYNC | UCMODE_3 | UCMST);

    // 7-bit addressing
    UCB1CTL0 &= ~UCSLA10;

    // Clock source: SMCLK:
    UCB1CTL1 |= UCSSEL__SMCLK;

    // Baud Rate registers for 100 kHz SCL, SMCLK = 8 MHz --> divider = 80
    UCB1BR0 = 80;
    UCB1BR1 = 0;


    // Put USCI out of software reset
    UCB1CTL1 &= ~UCSWRST;

}


int i2c_transfer(const i2c_device* device, const i2c_data* data) {

    int err = 0;

    // Set slave address of the device
    UCB1I2CSA = device->address;

    // Transmit data if there is any
    if(data->tx_length != 0) {

        err = transmit(device, (const uint8_t*) data->tx_buffer, data->tx_length);

    }

    // Receive data if there is any
    if( (err == 0) && (data->rx_length != 0)) {

        err = receive(device, (uint8_t*) data->rx_buffer, data->rx_length);

    } else {

       // No bytes to receive, send STOP condition
       UCB1CTL1 |= UCTXSTP;

    }

    return err;
}


static int transmit(const i2c_device* device, const uint8_t* buffer, size_t numberOfBytes) {

    int err = 0;

    // Set transmitter Mode and send START condition
    UCB1CTL1 |= (UCTR | UCTXSTT);

    // wait for start condition to be sent (UCTXSTT reset), wait for tx-buffer to be empty
    while( (UCB1CTL1 & UCTXSTT) && ((UCB1IFG & UCTXIFG) == 0) );

    // Check for slave ACK on initial byte
    err = check_acknowledge(device);

    // If no error and BYTES left to send, transmit data
    while( (err == 0) && (numberOfBytes > 0) ) {

        // send one byte from the buffer
        UCB1TXBUF = *buffer;

        // while tx-buffer is NOT empty, poll acknowledge
        while( ((UCB1IFG & UCTXIFG) == 0) ) {

            err = check_acknowledge(device);

            if(err < 0) {
                break;
            }
        }

        // tx-buffer empty, continue

        buffer++;
        numberOfBytes--;

    }

    return err;

}


static int receive(const i2c_device* device, uint8_t* buffer, size_t numberOfBytes) {

    int err = 0;

    // set receiver mode, send start condition
    UCB1CTL1 &= ~UCTR;
    UCB1CTL1 |= UCTXSTT;

    /* wait for start condition to be sent (UCTXSTT reset)
     * UCTXSTT resets when the first byte (slave address)
     * is sent
     */
    while( (UCB1CTL1 & UCTXSTT) );

    /* if there is only one BYTE to receive, then set the stop
     * bit as soon as START condition has been sent
     * NACK is automatically sent before STOP condition
    */
    if(numberOfBytes == 1) {
        UCB1CTL1 |= UCTXSTP;
    }

    // Check for ACK
    err = check_acknowledge(device);

    // If no error and BYTES left to receive, receive data
    while( (err == 0) && numberOfBytes != 0 ) {

        // Wait for the data to come in
        while( ((UCB1IFG & UCRXIFG) == 0) );

        // read receive buffer into current byte from buffer
        *buffer = UCB1RXBUF;

        buffer++;
        numberOfBytes--;

        // if there is only one BYTE to receive, then send STOP condition
        if(numberOfBytes == 1) {
            UCB1CTL1 |= UCTXSTP;
        }

    }

    return err;

}


static int check_acknowledge(const i2c_device* device) {

    int err = 0;

    // Check for NACK interrupt flag
    if( UCB1STAT & UCNACKIFG ) {

        // Stop the I2C transmission
        UCB1CTL1 |= UCTXSTP;

        // Clear interrupt flag
        UCB1STAT &= ~UCNACKIFG;

        // set error code
        err = -1;

    }

    return err;

}
