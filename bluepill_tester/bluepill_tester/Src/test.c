/*
 * test.c
 *
 *  Created on: Jul 23, 2018
 *      Author: kevinweiss
 */

#include "test.h"

#ifdef TEST_MODE

#include <string.h>

#include "stm32f1xx_hal.h"

#include "port_uart.h"
#include "app_shell_if.h"

#include "port.h"
#include "app_access.h"
#include "app_errno.h"
#include "app_typedef.h"
#include "app_common.h"
#include "app.h"
#include "app_debug.h"

void run_test() {
	uint32_t index = 152;
	for (uint64_t i = 0; i < 0xFFFFFFFFFFFFFFFF; i++) {
		if (increase_reg_uint8(index, IF_ACCESS) != EOK) test_failed();
		error_t increase_reg_uint8(uint32_t index, uint8_t access);
		error_t increase_reg_int8(uint32_t index, uint8_t access);
		error_t increase_reg_uint16(uint32_t index, uint8_t access);
		error_t increase_reg_int16(uint32_t index, uint8_t access);
		error_t increase_reg_uint32(uint32_t index, uint8_t access);
		error_t increase_reg_int32(uint32_t index, uint8_t access);
		error_t decrease_reg_uint8(uint32_t index, uint8_t access);
		error_t decrease_reg_int8(uint32_t index, uint8_t access);
		error_t decrease_reg_uint16(uint32_t index, uint8_t access);
		error_t decrease_reg_int16(uint32_t index, uint8_t access);
		error_t decrease_reg_uint32(uint32_t index, uint8_t access);
		error_t decrease_reg_int32(uint32_t index, uint8_t access);

	}
}

void test_failed() {
	while (1);
}

#endif
