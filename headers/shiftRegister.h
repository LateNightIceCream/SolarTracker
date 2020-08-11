/*
 * shift_register.h
 *
 *  Created on: Jul 12, 2020
 *      Author: zamza
 */

#ifndef HEADERS_SHIFTREGISTER_H_
#define HEADERS_SHIFTREGISTER_H_

#include <headers/clockConfiguration.h>
#include <stdint.h>

#define WIDTH_TIME_CYCLES ONE_US_CYCLES

typedef struct shiftReg {

    uint8_t* PORT_BASE_ADDRESS;

    uint8_t* rOUT;

    uint8_t DATA_BIT;
    uint8_t SRCLK_BIT;
    uint8_t RCLK_BIT; // latch bit
    uint8_t CLR_BIT;

    uint8_t content;

} shiftRegister;

void shiftRegister_init(shiftRegister* reg,
                        uint16_t       PORT_BASE_ADDRESS,
                        uint8_t        DATA_BIT,
                        uint8_t        SRCLK_BIT,
                        uint8_t        RCLK_BIT,
                        uint8_t        CLR_BIT);

void shiftRegister_write(shiftRegister* reg, uint8_t write_byte);

void shiftRegister_shift(shiftRegister* reg);

void shiftRegister_latch(shiftRegister* reg);

#endif /* HEADERS_SHIFTREGISTER_H_ */
