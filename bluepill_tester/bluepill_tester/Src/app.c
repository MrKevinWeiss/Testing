/*
 * app.c
 *
 *  Created on: May 16, 2018
 *      Author: kevinweiss
 */

#include <string.h>

#include "stm32f1xx_hal.h"


#include "app_typedef.h"
#include "build_defs.h"
#include "app_defaults.h"
#include "app_i2c.h"
#include "app.h"

#define FW_REV	(10000U)

static map_t reg = {0};

error_t _init_reg(){
	HAL_GetUID((uint32_t*)&reg.sys.sn[0]);
	reg.sys.fw_rev = FW_REV;

	reg.sys.build_time.second = BUILD_SEC;
	reg.sys.build_time.minute = BUILD_MIN;
	reg.sys.build_time.hour = BUILD_HOUR;
	reg.sys.build_time.day_of_month = BUILD_DAY;
	reg.sys.build_time.month = BUILD_MONTH;
	reg.sys.build_time.year = BUILD_YEAR;

	reg.i2c.slave_addr_1 = DEFAULT_I2C_SLAVE_ID_1;
	reg.i2c.slave_addr_2 = DEFAULT_I2C_SLAVE_ID_2;

	return EOK;
}

error_t execute_reg_change(){
	static map_t prev_reg = {0};
	static bool init = true;
	if (init){
		init = false;
		_init_reg();
	}
	else if (memcmp(&prev_reg.i2c, &reg.i2c, sizeof(reg.i2c))){
		app_i2c_execute(&reg.i2c);
	}

	memcpy(&prev_reg, &reg, sizeof(reg));
	return EOK;
}



uint32_t get_reg_size(){
	return sizeof(reg);
}

error_t read_reg(uint32_t index, uint8_t *data){
	if (index >= sizeof(reg)){
		return EOVERFLOW;
	}
	else{
		*data = reg.data8[index];
		return EOK;
	}
	return EUNKNOWN;
}

error_t write_cfg_reg(uint32_t index, uint8_t data){
	if (index < SYS_T_SIZE){
		return EACCES;
	}
	else if (index >= sizeof(reg)){
		return EOVERFLOW;
	}
	else{
		reg.data8[index] = data;
		return EOK;
	}
	return EUNKNOWN;
}

error_t write_user_reg(uint32_t index, uint8_t data){
	if (index < MAP_T_RES_OFFSET){
		return EACCES;
	}
	else if (index >= sizeof(reg)){
		return EOVERFLOW;
	}
	else{
		reg.data8[index] = data;
		return EOK;
	}
	return EUNKNOWN;
}
