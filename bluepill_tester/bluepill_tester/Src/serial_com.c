/*
 * serial_com.c
 *
 *  Created on: May 14, 2018
 *      Author: kevinweiss
 */

#include <string.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "mem_map.h"
#include "serial_com.h"

#define COM_BUF_SIZE	((uint16_t)256)

#define INIT_MSG		"Initializing Communication"

#define READ_BYTE_CMD	"rb "
#define READ_REG_CMD	"rr "
#define WRITE_REG_CMD	"wr "
#define EXECUTE_CMD		"e\n"

#define ATOU_MAX_CHAR	6
#define ATOU_ERROR		0xFFFFFFFF
#define BYTE_MAX		((uint8_t)0xFF)

#ifndef EOK
#define EOK 0
#endif

#ifndef EUNKNOWN
#define EUNKNOWN (__ELASTERROR + 1)
#endif

int32_t _get_rx_amount(UART_HandleTypeDef *huart);
uint32_t _parse_command(char *str);
uint32_t _fast_atou(char **str, char terminator);
int32_t _cmd_read_byte(char *str);
int32_t _cmd_read_reg(char *str);
int32_t _cmd_write_reg(char *str);
uint32_t _valid_args(char *str);


extern bpt_reg_t regs;
static UART_HandleTypeDef* huart_inst = NULL;

error_t app_com_init(UART_HandleTypeDef *huart) {

	error_t err =_tx_str(huart, INIT_MSG);
	if (err == EOK){
		huart_inst = huart;
	}

	return err;
}



error_t app_com_poll() {

	static char str[COM_BUF_SIZE] = { 0 };

	HAL_StatusTypeDef status = HAL_ERROR;
	int32_t rx_amount;

	if (huart_inst == NULL){
		return ENODEV;
	}

	if (huart_inst->RxState == HAL_UART_STATE_BUSY_RX){
		rx_amount = _get_rx_amount(huart_inst);
		if (rx_amount >= 1) {
			if (str[rx_amount - 1] == '\n') {
				HAL_UART_AbortTransmit(huart_inst);
				HAL_UART_AbortReceive(huart_inst);
				if (_parse_command(str) == 0){
					HAL_UART_Transmit_DMA(huart_inst, (uint8_t*)str, strlen(str));
				}
				else{
					sprintf(str, "1\r\n");
					HAL_UART_Transmit_DMA(huart_inst, (uint8_t*)str, strlen(str));
				}
				return 1; //error

			}
		}
	}
	else if (huart_inst->TxXferCount == 0){
		if (status == HAL_BUSY){
			HAL_UART_AbortTransmit(huart);
			HAL_UART_AbortReceive(huart);
			status = HAL_UART_Transmit_DMA(huart, INIT_MSG, sizeof(INIT_MSG));
			if (status == HAL_BUSY){
				return EBUSY;
			}
		}
		memset(str, 0, COM_BUF_SIZE);
		if (HAL_UART_Receive_DMA(huart_inst, (uint8_t*)str, COM_BUF_SIZE) != HAL_OK){
			return 1;
		}
	}
	return 0;
}


error_t _tx_str(UART_HandleTypeDef *huart, const char *str){
	HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(huart, str, strlen(str));
	if (status == HAL_BUSY){
		HAL_UART_AbortTransmit(huart);
		HAL_UART_AbortReceive(huart);
		status = HAL_UART_Transmit_DMA(huart, INIT_MSG, sizeof(INIT_MSG));
		if (status == HAL_BUSY){
			return EBUSY;
		}
	}
	if (status == HAL_ERROR){
		return ENXIO;
	}
	if (status == HAL_OK){
		return EOK;
	}
	return EUNKNOWN;
}

uint32_t _parse_command(char *str) {

	if (memcmp(str, READ_BYTE_CMD, strlen(READ_BYTE_CMD)) == 0) {
		return _cmd_read_byte(str);
	}
	else if (memcmp(str, READ_REG_CMD, strlen(READ_REG_CMD)) == 0){
		return _cmd_read_reg(str);
	}
	else if (memcmp(str, WRITE_REG_CMD, strlen(WRITE_REG_CMD)) == 0){
		return _cmd_write_reg(str);
	}

	return 1;
}

int32_t _cmd_read_byte(char *str) {
	char *arg_str = str + strlen(READ_BYTE_CMD);
	uint32_t index = _fast_atou(&arg_str, '\n');

	if (index >= 0 && index < sizeof(regs)) {
		DIS_INT;
		uint8_t data = regs.data_8[index];
		EN_INT;
		sprintf(str, "0,0x%02X\r\n", data);
		return 0;
	}
	return 1;
	//return ERROR
}

int32_t _cmd_read_reg(char *str) {
	char *arg_str = str + strlen(READ_REG_CMD);
	uint32_t index = _fast_atou(&arg_str, ' ');


	if (index >= 0 && index < sizeof(regs)){
		uint32_t size = _fast_atou(&arg_str, '\n');
		if (size >= 1 && size < sizeof(regs)){
			if ((size * 2) + 5 < COM_BUF_SIZE){
				uint8_t data;
				str += sprintf(str, "0,0x");
				index += size;
				if (index > sizeof(regs)){
					index -= sizeof(regs);
				}
				while (size > 0){
					index--;
					size--;


					DIS_INT;
					data = regs.data_8[index];
					EN_INT;
					if (index == 0){
						index = sizeof(regs);
					}


					//if overflow then stop
					str += sprintf(str, "%02X", data);
				}
				sprintf(str, "\r\n");
				return 0;
			}
		}
	}

	return 1;
	//return ERROR
}

int32_t _cmd_write_reg(char *str) {
	uint32_t arg_count = _valid_args(str);
	if (arg_count > 1){
		char *arg_str = str + strlen(WRITE_REG_CMD);
		uint32_t val;
		uint32_t index = _fast_atou(&arg_str, ' ');
		arg_count--;
		while (arg_count != 1){
			val = _fast_atou(&arg_str, ' ');
			DIS_INT;
			regs.data_8[index] = (uint8_t)val;
			EN_INT;
			index++;
			if (index == sizeof(regs)){
				index = 0;
			}
			arg_count--;
		}
		val = _fast_atou(&arg_str, '\n');
		DIS_INT;
		regs.data_8[index] = (uint8_t)val;
		EN_INT;
		sprintf(str, "0\r\n");
		return 0;
	}
	return 1;
	//return ERROR
}

uint32_t _valid_args(char *str){
	char *arg_str = str + strlen(WRITE_REG_CMD);
	uint32_t val;
	uint32_t arg_count = 0;

	while ((arg_str - str) < COM_BUF_SIZE - strlen(WRITE_REG_CMD)){
		char *end_check_str = arg_str;
		val = _fast_atou(&end_check_str, ' ');

		if (val == ATOU_ERROR){
			val = _fast_atou(&arg_str, '\n');
			if (val >= 0 && val < BYTE_MAX){
				return (arg_count + 1);
			}
			else {
				return 0;
			}
		}
		else if (val < 0 && val >= BYTE_MAX){
			return 0;
		}
		else{
			arg_str = end_check_str;
			arg_count++;
		}
	}
	return 0;
}


//must be used under controlled conditions, no error catching or handling
uint32_t _fast_atou(char **str, char terminator) {
	uint32_t val = 0;
	char *first_str = *str;

	if ((*str)[0] == '0' && (*str)[1] == 'x'){
		*str += 2;
		while (**str != terminator) {
			if ((uint32_t)(*str - first_str) >= ATOU_MAX_CHAR) {
				return ATOU_ERROR;
			}
			else if (**str >= '0' && **str <= '9') {
				val = (val<<4) + (**str - '0');
				(*str)++;
			}
			else if (**str >= 'a' && **str <= 'f') {
				val = (val<<4) + (**str - 'a');
				(*str)++;
			}
			else if (**str >= 'A' && **str <= 'F') {
				val = (val<<4) + (**str - 'A');
				(*str)++;
			}
			else{
				return ATOU_ERROR;
			}
		}
	}
	else{
		while (**str != terminator) {
			if ((uint32_t)(*str - first_str) >= ATOU_MAX_CHAR) {
				return ATOU_ERROR;
			} else if (**str < '0' && **str > '9') {
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

inline int32_t _get_rx_amount(UART_HandleTypeDef *huart) {
	return (COM_BUF_SIZE - huart->hdmarx->Instance->CNDTR);
}

void _App_Com_Rx_OverflowCallback(){

}

void _App_Com_ErrorCallback(){

}
