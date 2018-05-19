/*
 * app_common.c
 *
 *  Created on: May 18, 2018
 *      Author: kevinweiss
 */

#include "stm32f1xx_hal.h"

#include "app_common.h"

void _delay_ticks(uint64_t delay) {

	uint64_t ticks = 0;
	int32_t old_tick = SysTick->VAL;
	int32_t new_tick;
	int32_t diff;

	do{
		new_tick = SysTick->VAL;
		diff = old_tick - new_tick;
		if (diff < 0){
			diff += SysTick->LOAD;
		}
		ticks += diff;
		old_tick = new_tick;
	}while(ticks < delay);
}

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
	uint64_t freq = HAL_RCC_GetSysClockFreq()/1000000;
	freq *= micros;
	_delay_ticks(freq);//(uint64_t)((HAL_RCC_GetSysClockFreq()/1000000) * micros));
	/*
	uint32_t start = _get_us();
	while (_get_us() - start < (uint32_t) micros) {
		asm("nop");
	}
	*/

	//static int ii = 0;
	//for (int i = 0; i < micros; i++){

	//	ii++;
	//}
	//ii = 0;
}
