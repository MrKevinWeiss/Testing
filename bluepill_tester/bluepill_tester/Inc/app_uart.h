/*
 * app_uart.h
 *
 *  Created on: Jun 17, 2018
 *      Author: user
 */

#ifndef APP_UART_H_
#define APP_UART_H_

#include <errno.h>

#define MODE_ECHO		0x00
#define MODE_ECHO_EXT	0x01
#define MODE_REG		0x02

error_t app_uart_execute(uart_t *uart);
error_t app_uart_init(UART_HandleTypeDef *huart);
error_t app_uart_poll();

#endif /* APP_UART_H_ */
