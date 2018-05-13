/*
 * kinte_mem_map.h
 *
 *  Created on: May 13, 2018
 *      Author: kevinweiss
 */

#ifndef KINTE_MEM_MAP_H_
#define KINTE_MEM_MAP_H_

#if 0
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
#endif
#endif /* KINTE_MEM_MAP_H_ */
