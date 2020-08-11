/*
 * spa_helper.h
 *
 *  Created on: Jul 26, 2020
 *      Author: zamza
 */

#ifndef HEADERS_SPA_HELPER_H_
#define HEADERS_SPA_HELPER_H_

#include <headers/spa.h>
#include <stdint.h>
#include <headers/clockConfiguration.h>

int    spa_calculate_fast(spa_data* spa);
double delta_t_polynomial(uint8_t year);

#endif /* HEADERS_SPA_HELPER_H_ */
