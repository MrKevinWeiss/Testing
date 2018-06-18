/*
 * app_uart.h
 *
 *  Created on: Jun 17, 2018
 *      Author: user
 */

#ifndef APP_UART_H_
#define APP_UART_H_

#include <errno.h>

error_t app_uart_execute(uart_t *uart);

error_t app_uart_init(UART_HandleTypeDef *huart);

#endif /* APP_UART_H_ */
