/*
 * serial_comm.c
 *
 *  Created on: May 11, 2018
 *      Author: kevinweiss
 */
#if 0
#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"


#include "serial_comm.h"
#include "kinte_mem_map.h"

#define READ_BYTE_CMD	"rb "
#define READ_REG_CMD	"rr "
#define WRITE_REG_CMD	"wr "
#define EXECUTE_CMD		"e\n"

#define ATOU_MAX_CHAR	6
#define ATOU_ERROR		0xFFFFFFFF
#define BYTE_MAX		((uint8_t)0xFF)


extern kinte_reg_t regs __attribute__ ((aligned (8)));

int32_t _get_rx_amount(UART_HandleTypeDef *huart);
void _parse_command(char *str);
uint32_t _fast_atou(char * str, char terminator);
int32_t _cmd_read_byte(char *str);
int32_t _cmd_read_register(char *str);
int32_t _cmd_write_reg(char *str);

int init_comm(UART_HandleTypeDef *huart) {
	return 0;
}

int poll_comm(UART_HandleTypeDef *huart) {
	int32_t rx_amount = _get_rx_amount(huart);
	static char str[COMM_BUF_SIZE] = { 0 };

	//needs to init

	if (rx_amount >= 1) {
		if (str[rx_amount - 1] == '\n') {
			HAL_UART_AbortTransmit(huart);
			HAL_UART_AbortReceive(huart);
			_parse_command(str);

			//restart dma from zero if \n pressed

			//should only start once print is complete
			//if (HAL_UART_Receive_DMA(&huart, str, COMM_BUF_SIZE) != HAL_OK)
				return 1; //error

		}
	}
	return rx_amount;
}

void _parse_command(char *str) {

	if (memcmp(str, READ_BYTE_CMD, sizeof(READ_BYTE_CMD)) == 0) {
		_cmd_read_byte(str + sizeof(READ_BYTE_CMD));
	}
}

int32_t _cmd_read_byte(char *str) {
	uint32_t index = _fast_atou(str, '\n');

	if (index >= 0 && index < KINTE_REG_SIZE) {
		DIS_INT;
		uint8_t data = regs.data_8[index];
		EN_INT;
		sprintf(str, "0,0x%X\r\n", data);
		return 0;
	}
	return 1;
	//return ERROR
}

int32_t _cmd_read_register(char *str) {
	uint32_t index = _fast_atou(str, ' ');
	if (index >= 0 && index < KINTE_REG_SIZE){
		uint32_t size = _fast_atou(str, '\n');
		if (size >= 1 && size < KINTE_REG_SIZE){
			uint8_t data;
			str += sprintf(str, "0,0x");
			while (size > 0){
				DIS_INT;
				data = regs.data_8[index];
				EN_INT;
				index++;
				if (index == KINTE_REG_SIZE){
					index = 0;
				}
				size--;
				str += sprintf(str, "%X", data);
			}
			sprintf(str, "\r\n");
			return 0;
		}
	}

	return 1;
	//return ERROR
}

int32_t _cmd_write_reg(char *str) {
	uint32_t index = _fast_atou(str, ' ');

	if (index >= 0 && index < KINTE_REG_SIZE) {

		uint32_t val;

		val = _fast_atou(str, ' ');
		if (ATOU_ERROR){
			val = _fast_atou(str, '\n');
			if (val > 0 && val < BYTE_MAX){

			}
			else {
				return 1;
			}
		}

		if (val > 0 && val < BYTE_MAX){
			DIS_INT;
			regs.data_8[index] = val;
			EN_INT;
			sprintf(str, "0\r\n");
		}

		return 0;
	}
	return 1;
	//return ERROR
}


//must be used under controlled conditions, no error catching or handling
uint32_t _fast_atou(char * str, char terminator) {
	uint32_t index = 0;
	uint32_t val = 0;
	if (str[0] == '0' && str[1] == 'x'){
		str = str + 2;
		while (str[index] != terminator) {
			if (index >= ATOU_MAX_CHAR) {
				return ATOU_ERROR;
			}
			else if (str[index] >= '0' && str[index] <= '9') {
				val = (val<<4) + (str[index++] - '0');
			}
			else if (str[index] >= 'a' && str[index] <= 'f') {
				val = (val<<4) + (str[index++] - 'a');
			}
			else if (str[index] >= 'A' && str[index] <= 'F') {
				val = (val<<4) + (str[index++] - 'A');
			}
			else{
				return ATOU_ERROR;
			}
		}
	}
	else{
		while (str[index] != terminator) {
			if (index >= ATOU_MAX_CHAR) {
				return ATOU_ERROR;
			} else if (str[index] < '0' && str[index] > '9') {
				return ATOU_ERROR;
			} else {
				val = val * 10 + (str[index++] - '0');
			}
		}
	}
	return val;
}

inline int32_t _get_rx_amount(UART_HandleTypeDef *huart) {
	return (COMM_BUF_SIZE - huart->hdmarx->Instance->CNDTR);
}

#endif
