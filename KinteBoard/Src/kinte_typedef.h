/*
 * kinte_typedef.h
 *
 *  Created on: Apr 27, 2018
 *      Author: kevinweiss
 */

#ifndef KINTE_TYPEDEF_H_
#define KINTE_TYPEDEF_H_

#include <stdint.h>
#include <stdbool.h>

#define KINTE_REG_SIZE	128

typedef struct kinte_reg_t{
	union{
		uint8_t data_8[KINTE_REG_SIZE];
		uint8_t data_16[KINTE_REG_SIZE/2];
		uint8_t data_32[KINTE_REG_SIZE/4];
	};
}kinte_reg_t;

#endif /* KINTE_TYPEDEF_H_ */
