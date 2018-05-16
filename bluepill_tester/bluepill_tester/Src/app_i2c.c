/*
 * MIT License
 *
 * Copyright 2018 Kevin Weiss
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/*
 ******************************************************************************
 * @file           : app_i2c.c
 * @author         : Kevin Weiss
 * @date           : 16.05.2018
 * @brief          : Application specific I2C functionality.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32f1xx_hal.h"

#include "app_i2c.h"

/* Defines -------------------------------------------------------------------*/
#ifndef EOK
#define EOK 0
#endif

#ifndef EUNKNOWN
#define EUNKNOWN (__ELASTERROR + 1)
#endif

/* Private function prototypes -----------------------------------------------*/
static void _add_index(uint32_t *i);
static void _sub_index(uint32_t *i);
/* Private variables ---------------------------------------------------------*/
static uint8_t *reg = NULL;
static uint32_t reg_size = 0;
static uint32_t start_reg_index = 0;
static uint32_t reg_index = 0;
static bool set_index = false;
/**
 * @brief
 *
 * @retval errno defined error code.
 */
error_t app_i2c_init(I2C_HandleTypeDef *hi2c, uint8_t *p_reg, uint32_t size) {
	if (hi2c == NULL || p_reg == NULL){
		return ENODEV;
	}
	else{
		reg = p_reg;
		reg_size = size;
		reg_index = 0;
		start_reg_index = 0;
		hi2c->Instance->CR1 |= I2C_CR1_ACK;
		__HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
		return EOK;
	}
	return EUNKNOWN;
}



static HAL_StatusTypeDef I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c)
{
	/* Transfer Direction requested by Master */
	if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA) == RESET){
		//master is tx
		//slave will rx
		set_index = true;
	}
	else{
		//master is rx
		//slave must tx
		hi2c->Instance->DR = reg[reg_index];
		_add_index(&reg_index);
		//wait(2);
	}

	return HAL_OK;
}

void i2c_err(I2C_HandleTypeDef* hi2c) {

	uint32_t sr1itflags   = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources    = READ_REG(hi2c->Instance->CR2);

    if(((sr1itflags & I2C_FLAG_AF) != RESET) && ((itsources & I2C_IT_ERR) != RESET))
    {
    	_sub_index(&reg_index);
    }

    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
    //hi2c->Instance->CR1 &= ~I2C_CR1_ACK; //should I?
}

void i2c_it(I2C_HandleTypeDef* hi2c) {
	uint32_t sr2itflags   = READ_REG(hi2c->Instance->SR2);
	uint32_t sr1itflags   = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources    = READ_REG(hi2c->Instance->CR2);

	if(((sr1itflags & I2C_FLAG_ADDR) != RESET) && ((itsources & I2C_IT_EVT) != RESET))
	{
	  I2C_Slave_ADDR(hi2c);
	}
	else if (((sr1itflags & I2C_FLAG_STOPF) != RESET) && ((itsources & I2C_IT_EVT) != RESET)){
		__HAL_I2C_CLEAR_STOPFLAG(hi2c);
	}
	else if((sr2itflags & I2C_FLAG_TRA) != RESET){
		if(((sr1itflags & I2C_FLAG_TXE) != RESET) &&
				((itsources & I2C_IT_BUF) != RESET) &&
				((sr1itflags & I2C_FLAG_BTF) == RESET)){
			hi2c->Instance->DR = reg[reg_index];
			_add_index(&reg_index);
		}
		else if(((sr1itflags & I2C_FLAG_BTF) != RESET) &&
				((itsources & I2C_IT_EVT) != RESET)){
			//I2C_SlaveTransmit_BTF(hi2c);
		}
	}
	else if(((sr1itflags & I2C_FLAG_RXNE) != RESET) &&
			((itsources & I2C_IT_BUF) != RESET) &&
			((sr1itflags & I2C_FLAG_BTF) == RESET)){
		if (set_index == true){
			set_index = false;
			start_reg_index = hi2c->Instance->DR;
			reg_index = start_reg_index;
			//wait(3);
		}
		else{
			reg[reg_index] = hi2c->Instance->DR;
			_add_index(&reg_index);
			//sr1itflags   = READ_REG(hi2c->Instance->SR1);
			//wait(4);
		}

    }
	else if(((sr1itflags & I2C_FLAG_BTF) != RESET) && ((itsources & I2C_IT_EVT) != RESET))
	{
		_add_index(&reg_index);
		_add_index(&reg_index);
		//wait(5);
	}
}

static void _add_index(uint32_t *i){
	(*i)++;
	if (*i == reg_size){
		*i = 0;
	}
}

static void _sub_index(uint32_t *i){
	if (*i == 0){
		*i = reg_size;
	}
	(*i)--;
}
