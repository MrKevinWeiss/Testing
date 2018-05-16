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

#include "serial_com.h"

/* Defines -------------------------------------------------------------------*/
#ifndef EOK
#define EOK 0
#endif

#ifndef EUNKNOWN
#define EUNKNOWN (__ELASTERROR + 1)
#endif

/* Private function prototypes -----------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
static uint8_t *reg = NULL;
static uint32_t reg_size = 0;
static uint32_t reg_index = 0;

/**
 * @brief Initializes and attaches all the pointers for communication.
 *
 * @retval errno defined error code.
 */
error_t app_com_init(I2C_HandleTypeDef *hi2c, uint8_t *p_reg, uint32_t size) {
	if (hi2c == NULL || p_reg == NULL){
		return ENODEV;
	}
	else{
		reg = p_reg;
		reg_size = size;
		reg_index = 0;
		hi2c.Instance->CR1 |= I2C_CR1_ACK;
		__HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
		return EOK;
	}
	return EUNKNOWN;
}
