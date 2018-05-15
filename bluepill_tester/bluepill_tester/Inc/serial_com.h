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


int init_app_com(UART_HandleTypeDef *huart);
int poll_com();

void debug_print_com(char *str);

typedef enum
{
  SC_OK       = 0x00,
  SC_ABORT_TX_FAILED       = 0x01,
  SC_ABORT_RX_FAILED       = 0x02,

} SC_ERROR_CODE_t;

#endif /* SERIAL_COM_H_ */
