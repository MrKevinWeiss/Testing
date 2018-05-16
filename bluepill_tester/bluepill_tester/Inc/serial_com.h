/*
 * serial_com.h
 *
 *  Created on: May 14, 2018
 *      Author: kevinweiss
 */

#ifndef SERIAL_COM_H_
#define SERIAL_COM_H_

#define EN_INT
#define DIS_INT

error_t app_com_init(UART_HandleTypeDef *huart);
error_t app_com_poll();

void debug_print_com(char *str);



#endif /* SERIAL_COM_H_ */
