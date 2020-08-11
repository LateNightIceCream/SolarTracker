/*
 * spa_helper.c
 *
 *  Created on: Jul 26, 2020
 *      Author: zamza
 */

#include <headers/spa_helper.h>

//-------------- Helping functions for implementation (RG) --------------
/*
 * Polynomial to approximate the value of delta_t
 *  between the years 2015 and 2099, not using the month
 *
 * example call:
 *  delta_t_polynomial(20, 7)
 *
 * https://de.wikipedia.org/wiki/Delta_T#cite_note-Espenak.deltatpoly-5
 * */
double delta_t_polynomial(uint8_t year) {

    if((year < 15) || (year > 99)) {
        return 69.4; // fallback value: year 2020
    }

    // quadratic equation, not using the month value

        // integer method, very fast but no decimal places
        //uint32_t year32 = 4131850UL + 16072UL + 261UL * year * year;
        //return year32 >> 16;

        // int/flop method, relatively fast (500 cycles)
        uint32_t year32 = 4131850UL + 16072UL*year + 261UL * year * year;
        return year32 / 65536.0;
}


/*
 * Increases clock speed to make the heavy spa calculation faster
 *
 * */
int spa_calculate_fast(spa_data* spa) {

    UCS_initFLLSettle(MCLK_SPEED_OC_KHZ, MCLK_FLLREF_OC_RATIO);
    int result = spa_calculate(spa);
    UCS_initFLLSettle(MCLK_SPEED_KHZ, MCLK_FLLREF_RATIO);

    return result;

}

