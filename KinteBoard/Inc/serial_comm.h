/*
 * serial_comm.h
 *
 *  Created on: May 11, 2018
 *      Author: kevinweiss
 */

#ifndef SERIAL_COMM_H_
#define SERIAL_COMM_H_

#define COMM_BUF_SIZE	256

int init_comm(UART_HandleTypeDef *huart);
int poll_comm(UART_HandleTypeDef *huart);

void print_comm(char *str);

typedef enum
{
  SC_OK       = 0x00U,
  SC_ABORT_TRANSMIT_FAILED       = 0x01U,
  SC_ABORT_TRANSMIT_FAILED       = 0x01U,
  SC_ABORT_TRANSMIT_FAILED       = 0x01U,
  SC_OK       = 0x00U,

} HAL_StatusTypeDef;


#endif /* SERIAL_COMM_H_ */
