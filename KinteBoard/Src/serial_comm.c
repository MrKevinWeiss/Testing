/*
 * serial_comm.c
 *
 *  Created on: May 11, 2018
 *      Author: kevinweiss
 */


#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "serial_comm.h"

#define READ_REG_CMD	"r "
#define WRITE_REG_CMD	"w "
#define CMD_MAX_SIZE	3


inline int32_t _get_rx_amount(UART_HandleTypeDef *huart);
inline void _parse_command(char *str);

int init_comm(UART_HandleTypeDef *huart){

}


int poll_comm(UART_HandleTypeDef *huart)
{
	int32_t rx_amount = _get_rx_amount(huart);
	static char str[COMM_BUF_SIZE] = {0};

	//needs to init

	if (rx_amount >= 1){
		if (str[rx_amount -1] == '\n'){
			HAL_UART_AbortTransmit(&huart);
			HAL_UART_AbortReceive(&huart);
			_parse_command(str);
			//restart dma from zero if \n pressed

			if (HAL_UART_Receive_DMA(&huart, str, COMM_BUF_SIZE) != HAL_OK) return 1;//error


		}
	}
	return rx_amount;
}

inline void _parse_command(const char *str)
{
	uint32_t len = strlen(str);

	if (len >= CMD_MAX_SIZE)
	{
		if (memcmp(str, READ_REG_CMD, sizeof(READ_REG_CMD)) == 0)
		{

		}
	}
}


int32_t fast_atou(const char * str, char terminator, uint32_t size)
{
    int32_t val = 0;
    while(*str != terminator) {
    	if (size-- == 0) return -1;
        val = val*10 + (*str++ - '0');
    }
    return val;
}

inline int32_t _cmd_read_reg(const char *str){

}

inline int32_t _get_rx_amount(UART_HandleTypeDef *huart)
{
	return (COMM_BUF_SIZE - huart->hdmarx->Instance->CNDTR);
}


