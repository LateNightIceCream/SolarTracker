/*
 * shiftregister.c
 *
 *  Created on: Jul 12, 2020
 *      Author: zamza
 */

#include <headers/shiftRegister.h>

//////////////////////////////////
/// Initialization

void shiftRegister_init(shiftRegister* reg,
                         uint16_t      PORT_BASE_ADDRESS,
                         uint8_t       DATA_BIT,
                         uint8_t       SRCLK_BIT,
                         uint8_t       RCLK_BIT,
                         uint8_t       CLR_BIT) {

    // some ports share a base address (like P3 and P4)
    // so be sure to call function with right base offset
    // (see datasheet: peripheral file map)
    reg->PORT_BASE_ADDRESS = (uint8_t*) PORT_BASE_ADDRESS;

    reg->rOUT = (reg->PORT_BASE_ADDRESS + 0x02);

    reg->DATA_BIT  = DATA_BIT;
    reg->SRCLK_BIT = SRCLK_BIT;
    reg->RCLK_BIT  = RCLK_BIT;
    reg->CLR_BIT   = CLR_BIT;

    // initialize output state to 0
    *(reg->rOUT) &= ~(DATA_BIT | SRCLK_BIT | RCLK_BIT | CLR_BIT);

    // Direction Register, OUTPUT
    *(reg->PORT_BASE_ADDRESS + 0x04) |= (DATA_BIT | SRCLK_BIT | RCLK_BIT | CLR_BIT);

    // Selection Register, I/O
    *(reg->PORT_BASE_ADDRESS + 0x0A) &= ~(DATA_BIT | SRCLK_BIT | RCLK_BIT | CLR_BIT);

    __delay_cycles(WIDTH_TIME_CYCLES);

    // pulse RCLK
    *(reg->rOUT) |= RCLK_BIT;
    __delay_cycles(WIDTH_TIME_CYCLES);
    *(reg->rOUT) &= ~RCLK_BIT;

    // pull clear back up high (active low)
    *(reg->rOUT) |= CLR_BIT;

}


//////////////////////////////////
/// Write Byte to Register

void shiftRegister_write(shiftRegister* reg, uint8_t write_byte) {

    *(reg->rOUT) &= ~(reg->RCLK_BIT);

    // write data, MSB first
    for(int i = 7; i >= 0; i--) {

        // check if current bit in write_byte == 1, then put bit on port
        if( write_byte & (1 << i) ) {
            *(reg->rOUT) |= reg->DATA_BIT;
        } else {
            *(reg->rOUT) &= ~(reg->DATA_BIT);
        }

        // setup time?
        //__delay_cycles(WIDTH_TIME_CYCLES);

        // pulse clock
        *(reg->rOUT) |= (reg->SRCLK_BIT);
        //__delay_cycles(WIDTH_TIME_CYCLES);
        *(reg->rOUT) &= ~(reg->SRCLK_BIT);

    }


    // pulse RCLK to latch output after writing data
    *(reg->rOUT) |= (reg->RCLK_BIT);
    //__delay_cycles(WIDTH_TIME_CYCLES);
    *(reg->rOUT) &= ~(reg->RCLK_BIT); // maybe not neccessarry

    // update content variable
    reg->content = write_byte;

}

void shiftRegister_shift(shiftRegister* reg) {

    *(reg->rOUT) |= (reg->SRCLK_BIT);
    *(reg->rOUT) &= ~(reg->SRCLK_BIT);

}

void shiftRegister_latch(shiftRegister* reg) {

    *(reg->rOUT) |= (reg->RCLK_BIT);
    *(reg->rOUT) &= ~(reg->RCLK_BIT);

}
