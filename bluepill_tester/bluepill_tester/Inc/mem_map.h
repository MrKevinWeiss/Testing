/*
 * mem_map.h
 *
 *  Created on: May 14, 2018
 *      Author: kevinweiss
 */

#ifndef MEM_MAP_H_
#define MEM_MAP_H_

#include <stdint.h>
#include <stdbool.h>

#define BPT_REG_SIZE	128

typedef struct bpt_reg_t{
	union{
		uint8_t data_8[BPT_REG_SIZE];
		uint16_t data_16[BPT_REG_SIZE/2];
		uint32_t data_32[BPT_REG_SIZE/4];
	};
}bpt_reg_t;

#endif /* MEM_MAP_H_ */
