/*
 * clock_config.c
 *
 *  Created on: Jul 13, 2020
 *      Author: zamza
 */

#include <headers/clockConfiguration.h>

void clock_init(void) {

    // Set DCO FLL reference to REFO
    UCS_initClockSignal(UCS_FLLREF,
                        UCS_REFOCLK_SELECT,
                        UCS_CLOCK_DIVIDER_1);

    UCS_initFLLSettle(MCLK_SPEED_KHZ, MCLK_FLLREF_RATIO);

}
