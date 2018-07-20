/*
 * app.c
 *
 *  Created on: May 16, 2018
 *      Author: kevinweiss
 */

#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#include "stm32f1xx_hal.h"

#include "app_access.h"
#include "app_errno.h"
#include "app_typedef.h"
#include "app_common.h"
#include "build_defs.h"
#include "app_defaults.h"
#include "app_i2c.h"
#include "port_dut_uart.h"
#include "app.h"

static map_t reg = { 0 };

//TODO: Add i2c last amount of writes and reads
//TODO: Support 16/8 bit address confirm
//TODO: Add i2c bus reset or some kind of way to handle master collision detect
//TODO: Write with repeated start vs no repeated start

error_t _init_reg(map_t *reg_to_init) {
	DIS_INT;
	memset(reg_to_init->data8, 0, sizeof(*reg_to_init));

	HAL_GetUID((uint32_t*) &reg_to_init->sys.sn[0]);
	reg.sys.fw_rev = FW_REV;

	reg_to_init->sys.build_time.second = BUILD_SEC;
	reg_to_init->sys.build_time.minute = BUILD_MIN;
	reg_to_init->sys.build_time.hour = BUILD_HOUR;
	reg_to_init->sys.build_time.day_of_month = BUILD_DAY;
	reg_to_init->sys.build_time.month = BUILD_MONTH;
	reg_to_init->sys.build_time.year = BUILD_YEAR;

	reg_to_init->sys.device_num = DEFAULT_SYS_DEVICE_NUM;

	reg_to_init->i2c.slave_addr_1 = DEFAULT_I2C_SLAVE_ID_1;
	reg_to_init->i2c.slave_addr_2 = DEFAULT_I2C_SLAVE_ID_2;

	reg_to_init->i2c.clk_stretch_delay = 0x000;

	reg_to_init->uart.baud = DEFAULT_UART_BAUDRATE;
	reg_to_init->uart.rx_count = 0;
	reg_to_init->uart.tx_count = 0;
	reg_to_init->uart.ctrl.parity = DEFAULT_UART_PARITY;
	reg_to_init->uart.ctrl.stop_bits = DEFAULT_UART_STOP_BITS;

	for (int i = 0; i < sizeof(reg_to_init->user_reg); i++) {
		reg_to_init->user_reg[i] = (uint8_t) i;
	}
	EN_INT;
	return EOK;
}

void app_sys_execute(sys_t *sys) {
	if (sys->cr.dut_rst) {
		HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_SET);
	}
}

error_t execute_reg_change() {
	static map_t prev_reg = { 0 };
	static bool init = true;

	if (init) {
		init = false;
		_init_reg(&reg);
	}

	if (memcmp(&prev_reg.sys, &reg.sys, sizeof(reg.sys))) {
		app_sys_execute(&reg.sys);
	}

	if (memcmp(&prev_reg.i2c, &reg.i2c, sizeof(reg.i2c))) {
		app_i2c_execute(&reg.i2c);
	}

	if (memcmp(&prev_reg.uart, &reg.uart, sizeof(reg.uart))) {
		uart_dut_execute(&reg.uart);
	}

	memcpy(&prev_reg, &reg, sizeof(reg));
	return EOK;
}

uint32_t get_reg_size() {
	return sizeof(reg);
}

error_t read_reg(uint32_t index, uint8_t *data) {
	if (index >= sizeof(reg)) {
		return EOVERFLOW;
	} else {
		*data = reg.data8[index];
		return EOK;
	}
	return EUNKNOWN;
}

error_t read_regs(uint32_t index, uint8_t *data, uint16_t size) {
    error_t errcode = EUNKNOWN;
    for (int i = 0; i < size; i++) {
        DIS_INT;
        errcode = read_reg(index + i, &data[i]);
        EN_INT;
        if (errcode != EOK)
            return errcode;
    }
    return errcode;
}

error_t write_reg(uint32_t index, uint8_t data, uint8_t access) {

	if (index >= sizeof(reg)) {
		return EOVERFLOW;
	} else if (!(REG_ACCESS[index] & access)) {
		return EACCES;
	} else {
		reg.data8[index] = data;
		return EOK;
	}
	return EUNKNOWN;
}

error_t write_regs(uint32_t index, uint8_t *data, uint16_t size, uint8_t access) {
    error_t errcode = EUNKNOWN;
    for (int i = 0; i < size; i++) {
        DIS_INT;
        errcode = write_reg(index + i, data[i], access);
        EN_INT;
        if (errcode != EOK)
            return errcode;
    }
    return errcode;
}
