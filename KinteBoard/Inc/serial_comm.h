/*
 * serial_comm.h
 *
 *  Created on: May 11, 2018
 *      Author: kevinweiss
 */

#ifndef SERIAL_COMM_H_
#define SERIAL_COMM_H_


#if 0
#define EN_INT
#define DIS_INT
#define COMM_BUF_SIZE	256

int init_comm(UART_HandleTypeDef *huart);
int poll_comm(UART_HandleTypeDef *huart);

void print_comm(char *str);

typedef enum
{
  SC_OK       = 0x00,
  SC_ABORT_TX_FAILED       = 0x01,
  SC_ABORT_RX_FAILED       = 0x02,

} SC_ERROR_CODE_t;
#endif

#endif /* SERIAL_COMM_H_ */
