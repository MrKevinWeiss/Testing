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
 * @file           : app_uart.c
 * @author         : Yegor Yefremov
 * @date           : 16.06.2018
 * @brief          : Application specific UART functionality.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "app_errno.h"
#include "app_common.h"
#include "app_errno.h"
#include "app_typedef.h"
#include "app.h"
#include "app_debug.h"

/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef *huart1_inst;

/**
 * @brief Initializes and attaches all the pointers for communication.
 *
 * @retval errno defined error code.
 */
error_t app_uart_init(UART_HandleTypeDef *huart) {
	huart1_inst = huart;
	return EOK;
}

/**
 * @brief
 *
 * @retval errno defined error code.
 */
error_t app_uart_execute(uart_t *uart) {
	huart1_inst->Init.BaudRate = uart->baud;
	return EOK;
}
