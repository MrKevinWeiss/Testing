/*
 * app_common.c
 *
 *  Created on: May 18, 2018
 *      Author: kevinweiss
 */

#include "stm32f1xx_hal.h"

#include "app_common.h"

uint32_t _get_us(void) {
	uint32_t usTicks = HAL_RCC_GetSysClockFreq() / 1000000;
	register uint32_t ms, cycle_cnt;
	do {
		ms = HAL_GetTick();
		cycle_cnt = SysTick->VAL;
	} while (ms != HAL_GetTick());
	return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

void delay_us(uint16_t micros) {
	uint32_t start = _get_us();
	while (_get_us() - start < (uint32_t) micros) {
		asm("nop");
	}
}
