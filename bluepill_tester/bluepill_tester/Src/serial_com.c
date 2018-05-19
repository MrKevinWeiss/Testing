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
 * @file           : serial_com.c
 * @author         : Kevin Weiss
 * @date           : 16.05.2018
 * @brief          : Serial communication handling.
 ******************************************************************************
 *
 * This initializes and runs the serial communication protocol for interfacing
 * to registers.  It used the STM HAL and a UART for the IO.  It also uses DMA.
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "stm32f1xx_hal.h"

#include "app_typedef.h"
#include "app_common.h"
#include "app.h"
#include "app_debug.h"
#include "serial_com.h"

/* Defines -------------------------------------------------------------------*/
#define COM_BUF_SIZE	((uint16_t)1024)//((uint16_t)256)

#define RX_END_CHAR		'\n'
#define TX_END_STR		"\n"
#define READ_BYTE_CMD	"rb "
#define READ_REG_CMD	"rr "
#define WRITE_REG_CMD	"wr "
#define EXECUTE_CMD		"ex\n"
#define RESET_CMD		"mcu_rst\n"

#define ATOU_MAX_CHAR	5
#define ATOU_ERROR		0xFFFFFFFF
#define BYTE_MAX		((uint8_t)0xFF)

#define IS_RX_WAITING(x)	(HAL_IS_BIT_SET(x, USART_CR3_DMAR))
#define IS_NUM(x)			(x >= '0' && x <= '9')



/* Private function prototypes -----------------------------------------------*/
static error_t _tx_str(UART_HandleTypeDef *huart, char *str);
static error_t _rx_str(UART_HandleTypeDef *huart, char *str);
static error_t _xfer_complete(UART_HandleTypeDef *huart, char *str);

static error_t _parse_command(char *str);
static error_t _cmd_read_byte(char *str);
static error_t _cmd_read_reg(char *str);
static error_t _cmd_write_reg(char *str);
static error_t _cmd_execute(char *str);
static error_t _cmd_reset();

static error_t _valid_args(char *str, uint32_t *arg_count);
static uint32_t _fast_atou(char **str, char terminator);
static inline int32_t _get_rx_amount(UART_HandleTypeDef *huart);

/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef* huart_inst = NULL;

/**
 * @brief Initializes and attaches all the pointers for communication.
 *
 * @retval errno defined error code.
 */
error_t app_com_init(UART_HandleTypeDef *huart) {
	char str[COM_BUF_SIZE] = {0};
	sprintf(str, "Build Date: %s %s\n", __DATE__, __TIME__);
	error_t err = _tx_str(huart, str);
	if (err == EOK) {
		huart_inst = huart;
	}
	return err;
}

/**
 * @brief Polls and parses the rx and tx buffers.
 *
 * @retval errno defined error code.
 */
error_t app_com_poll() {
	static char str[COM_BUF_SIZE] = { 0 };
	error_t err = ENOACTION;

	if (IS_RX_WAITING(huart_inst->Instance->CR3)) {
		err = _rx_str(huart_inst, str);
	} else if (huart_inst->TxXferCount == 0) {
		if (str[COM_BUF_SIZE - 1] != 0) {
			err = EMSGSIZE;
			HAL_UART_AbortTransmit(huart_inst);
			HAL_UART_AbortReceive(huart_inst);
			memset(str, 0, COM_BUF_SIZE);
			sprintf(str, "%d%s", err, TX_END_STR);
			HAL_UART_Transmit_DMA(huart_inst, (uint8_t*) str, strlen(str));
		} else {
			err = _xfer_complete(huart_inst, str);
		}
	}
	return err;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _xfer_complete(UART_HandleTypeDef *huart, char *str) {
	HAL_StatusTypeDef status;
	error_t err = EUNKNOWN;

	memset(str, 0, COM_BUF_SIZE);
	HAL_UART_AbortTransmit(huart_inst);
	HAL_UART_AbortReceive(huart_inst);
	status = HAL_UART_Receive_DMA(huart, (uint8_t*) str, COM_BUF_SIZE);
	if (status == HAL_BUSY) {
		err = EBUSY;
	}
	if (status == HAL_ERROR) {
		err = ENXIO;
	} else if (status == HAL_OK) {
		err = EOK;
	}
	return err;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _rx_str(UART_HandleTypeDef *huart, char *str) {
	int32_t rx_amount;
	error_t err = ENOACTION;
	rx_amount = _get_rx_amount(huart);
	if (rx_amount >= 1) {
		if (str[rx_amount - 1] == RX_END_CHAR
				&& _get_rx_amount(huart) != COM_BUF_SIZE) {
			HAL_UART_AbortTransmit(huart);
			HAL_UART_AbortReceive(huart);
			err = _parse_command(str);
			if (err == EOK) {
				HAL_UART_Transmit_DMA(huart, (uint8_t*) str, strlen(str));
			} else {
				sprintf(str, "%d%s", err, TX_END_STR);
				HAL_UART_Transmit_DMA(huart, (uint8_t*) str, strlen(str));
			}
		}
	}
	return err;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _tx_str(UART_HandleTypeDef *huart, char *str) {
	error_t err = EUNKNOWN;
	HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(huart, (uint8_t*) str,
			strlen(str));

	if (status == HAL_BUSY) {
		HAL_UART_AbortTransmit(huart);
		HAL_UART_AbortReceive(huart);
		status = HAL_UART_Transmit_DMA(huart, (uint8_t*) str, strlen(str));
		if (status == HAL_BUSY) {
			err = EBUSY;
		}
	}
	if (status == HAL_ERROR) {
		err = ENXIO;
	} else if (status == HAL_OK) {
		err = EOK;
	}

	return err;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _parse_command(char *str) {
	if (memcmp(str, READ_BYTE_CMD, strlen(READ_BYTE_CMD)) == 0) {
		return _cmd_read_byte(str);
	}
	else if (memcmp(str, READ_REG_CMD, strlen(READ_REG_CMD)) == 0) {
		return _cmd_read_reg(str);
	}
	else if (memcmp(str, WRITE_REG_CMD, strlen(WRITE_REG_CMD)) == 0) {
		return _cmd_write_reg(str);
	}
	else if (memcmp(str, EXECUTE_CMD, strlen(EXECUTE_CMD)) == 0) {
		return _cmd_execute(str);
	}
	else if (memcmp(str, RESET_CMD, strlen(RESET_CMD)) == 0) {
		return _cmd_reset();
	}

	return EPROTONOSUPPORT;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _cmd_read_byte(char *str) {
	char *arg_str = str + strlen(READ_BYTE_CMD);
	uint32_t index = _fast_atou(&arg_str, RX_END_CHAR);

	if (index == ATOU_ERROR) {
		return EINVAL;
	} else if (index >= get_reg_size()) {
		return EOVERFLOW;
	} else {
		uint8_t data;
		DIS_INT;
		read_reg(index, &data);
		EN_INT;
		sprintf(str, "0,0x%02X%s", data, TX_END_STR);
		return EOK;
	}
	return EUNKNOWN;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _cmd_read_reg(char *str) {
	char *first_str = str;
	char *arg_str = str + strlen(READ_REG_CMD);
	uint32_t index = _fast_atou(&arg_str, ' ');

	if (index == ATOU_ERROR) {
		return EINVAL;
	} else if (index >= get_reg_size()) {
		return EOVERFLOW;
	} else {
		uint32_t size = _fast_atou(&arg_str, RX_END_CHAR);
		if (size == ATOU_ERROR) {
			return EINVAL;
		} else if ((size * 2) + strlen(TX_END_STR)
				+ strlen("0,0x") >= COM_BUF_SIZE) {
			return ERANGE;
		} else {
			uint8_t data;
			str += sprintf(str, "%d,0x", EOK);
			index += size;
			if (index > get_reg_size()) {
				index -= get_reg_size();
			}
			while (size > 0) {
				index--;
				size--;

				DIS_INT;
				read_reg(index, &data);
				EN_INT;

				if (index == 0) {
					index = get_reg_size();
				}
				if ((str - first_str) + 2 + strlen(TX_END_STR) < COM_BUF_SIZE) {
					str += sprintf(str, "%02X", data);
				} else {
					return ERANGE;
				}
			}
			sprintf(str, TX_END_STR);
			return EOK;
		}
	}
	return EUNKNOWN;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _cmd_write_reg(char *str) {
	uint32_t arg_count = 0;
	error_t err;

	err = _valid_args(str, &arg_count);
	if (err == EOK) {
		if (arg_count < 2) {
			err = ENODATA;
		} else {
			char *arg_str = str + strlen(WRITE_REG_CMD);
			uint32_t val;
			uint32_t index = _fast_atou(&arg_str, ' ');
			if (index >= get_reg_size()){
				err = EOVERFLOW;
			}
			else {
				arg_count--;
				while (arg_count != 1) {
					val = _fast_atou(&arg_str, ' ');
					DIS_INT;
					err = write_cfg_reg(index, val);
					EN_INT;
					if (err != EOK){
						return err;
					}
					index++;
					if (index == get_reg_size()) {
						index = 0;
					}
					arg_count--;
				}
				val = _fast_atou(&arg_str, RX_END_CHAR);
				DIS_INT;
				err = write_cfg_reg(index, val);
				EN_INT;
				if (err == EOK){
					sprintf(str, "%d%s", EOK, TX_END_STR);
				}
			}
		}
	}
	return err;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _cmd_execute(char *str) {

	error_t err = execute_reg_change();
	if (err == EOK){
		sprintf(str, "%d%s", EOK, TX_END_STR);
	}
	return err;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _valid_args(char *str, uint32_t *arg_count) {
	char *arg_str = str + strlen(WRITE_REG_CMD);
	uint32_t val;

	*arg_count = 0;
	while ((arg_str - str) < COM_BUF_SIZE - strlen(WRITE_REG_CMD)) {
		char *end_check_str = arg_str;
		val = _fast_atou(&end_check_str, ' ');
		if (val == ATOU_ERROR) {
			val = _fast_atou(&arg_str, RX_END_CHAR);
			if (val == ATOU_ERROR) {
				return EINVAL;
			} else if (val > BYTE_MAX) {
				return EOVERFLOW;
			} else {
				(*arg_count)++;
				return EOK;
			}
		} else if (val > BYTE_MAX) {
			return EOVERFLOW;
		} else {
			arg_str = end_check_str;
			(*arg_count)++;
		}
	}
	return EMSGSIZE;
}

static error_t _cmd_reset(){
	SOFT_RESET;
	return EUNKNOWN;
}

/**
 * @brief Private function, only handles controlled inputs.
 *
 * @retval numerical value from the string.
 */
uint32_t _fast_atou(char **str, char terminator) {
	uint32_t val = 0;
	char *first_str = *str;

	if (**str == terminator) {
		return ATOU_ERROR;
	}

	if ((*str)[0] == '0' && (*str)[1] == 'x') {
		*str += 2;
		while (**str != terminator) {
			if ((uint32_t) (*str - first_str) >= ATOU_MAX_CHAR) {
				return ATOU_ERROR;
			} else if (IS_NUM(**str)) {
				val = (val << 4) + (**str - '0');
				(*str)++;
			} else if (**str >= 'a' && **str <= 'f') {
				val = (val << 4) + (**str - 'a' + 10);
				(*str)++;
			} else if (**str >= 'A' && **str <= 'F') {
				val = (val << 4) + (**str - 'A' + 10);
				(*str)++;
			} else {
				return ATOU_ERROR;
			}
		}
	} else {
		while (**str != terminator) {
			if ((uint32_t) (*str - first_str) >= ATOU_MAX_CHAR) {
				return ATOU_ERROR;
			} else if (!IS_NUM(**str)) {
				return ATOU_ERROR;
			} else {
				val = val * 10 + (**str - '0');
				(*str)++;
			}
		}
	}
	(*str)++;
	return val;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static inline int32_t _get_rx_amount(UART_HandleTypeDef *huart) {
	return (COM_BUF_SIZE - huart->hdmarx->Instance->CNDTR);
}

void debug_print(char *str){
#ifdef DEBUG_PRINT
	HAL_UART_AbortTransmit(huart_inst);
	HAL_UART_Transmit_DMA(huart_inst, (uint8_t*) str, strlen(str));
#endif
}
