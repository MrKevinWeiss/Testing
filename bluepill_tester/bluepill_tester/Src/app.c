/*
 * app.c
 *
 *  Created on: May 16, 2018
 *      Author: kevinweiss
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32f1xx_hal.h"

#include "app_errno.h"
#include "app_typedef.h"
#include "app.h"

#define FW_REV	(10000U)


static map_t reg = {0};

error_t init_reg(){
	HAL_GetUID(&reg.sys.sn);
	reg.sys.fw_rev = FW_REV;
	return EOK;
}

error_t execute_reg_change(){
	static map_t prev_reg = {0};
	static init = true;
	if (init){
		init = false;
	}
	else if (memcmp(&prev_reg.i2c, &reg.i2c, sizeof(reg.i2c))){

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
