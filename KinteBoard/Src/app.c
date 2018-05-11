/*
 * app.c
 *
 *  Created on: Apr 27, 2018
 *      Author: kevinweiss
 */

#include "stm32f1xx_hal.h"

#include "kinte_typedef.h"

#include "app.h"

#define START_REG	0

kinte_reg_t regs __attribute__ ((aligned (8)));;
extern I2C_HandleTypeDef hi2c1;

#define MAX_REG_DATA_MSK	0x7F

#define REG_ADD(x)	(x=((x + 1) & MAX_REG_DATA_MSK))
#define REG_SUB(x)	(x=((x - 1) & MAX_REG_DATA_MSK))

int i2cpointer = 0;
void wait(int time){
	for (int i = 0; i < time; i++){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
		}
	for (int i = 0; i < 5; i++){
			static int i =0;
			i++;
	}
}
int flag_test1 = 0;

#if 1
void init_app() {
	int i;
	for (i = 0; i < KINTE_REG_SIZE; i++) {
		regs.data_8[i] = i;// + '0';
	}

	wait(100);
	hi2c1.Instance->CR1 |= I2C_CR1_ACK;

	__HAL_I2C_ENABLE_IT(&hi2c1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

}



static HAL_StatusTypeDef I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c)
{
	/* Transfer Direction requested by Master */
	if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA) == RESET)
	{
		//master is tx
		//slave will rx
		flag_test1 = 1;

	}
	else{
		//master is rx
		//slave must tx
		hi2c->Instance->DR = regs.data_8[i2cpointer];
		REG_ADD(i2cpointer);
		//wait(2);
	}
#if 0
	if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_DUALF) == RESET)
	{
		SlaveAddrCode = hi2c->Init.OwnAddress1;
	}
	else
	{
		SlaveAddrCode = hi2c->Init.OwnAddress2;
	}
#endif

	return HAL_OK;
}

void i2c_err(I2C_HandleTypeDef* hi2c)
{
	//wait(2);
	//uint32_t sr2itflags   = READ_REG(hi2c->Instance->SR2);
	uint32_t sr1itflags   = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources    = READ_REG(hi2c->Instance->CR2);



    if(((sr1itflags & I2C_FLAG_AF) != RESET) && ((itsources & I2C_IT_ERR) != RESET))
    {
    	REG_SUB(i2cpointer);
    }

    /* Clear AF flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Disable Acknowledge? */
    //hi2c->Instance->CR1 &= ~I2C_CR1_ACK;
}

void i2c_it(I2C_HandleTypeDef* hi2c){
	//wait(1);
	uint32_t sr2itflags   = READ_REG(hi2c->Instance->SR2);
	uint32_t sr1itflags   = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources    = READ_REG(hi2c->Instance->CR2);

	if(((sr1itflags & I2C_FLAG_ADDR) != RESET) && ((itsources & I2C_IT_EVT) != RESET))
	{
	  I2C_Slave_ADDR(hi2c);
	}
	else if (((sr1itflags & I2C_FLAG_STOPF) != RESET) && ((itsources & I2C_IT_EVT) != RESET)){
		//wait(6);
		__HAL_I2C_CLEAR_STOPFLAG(hi2c);
	}
	else if((sr2itflags & I2C_FLAG_TRA) != RESET)
	{
		//in transmit mode
		if(((sr1itflags & I2C_FLAG_TXE) != RESET) && ((itsources & I2C_IT_BUF) != RESET) && ((sr1itflags & I2C_FLAG_BTF) == RESET))
		{
			hi2c->Instance->DR = regs.data_8[i2cpointer];
			REG_ADD(i2cpointer);
			//wait(1);
		}
		else if(((sr1itflags & I2C_FLAG_BTF) != RESET) && ((itsources & I2C_IT_EVT) != RESET))
		{
			//I2C_SlaveTransmit_BTF(hi2c);
		}
	}
	else if(((sr1itflags & I2C_FLAG_RXNE) != RESET) && ((itsources & I2C_IT_BUF) != RESET) && ((sr1itflags & I2C_FLAG_BTF) == RESET))
    {
		if (flag_test1){
			flag_test1 = 0;
			i2cpointer = hi2c->Instance->DR;
			//wait(3);
		}
		else{
			regs.data_8[i2cpointer] = hi2c->Instance->DR;
			REG_ADD(i2cpointer);
			//sr1itflags   = READ_REG(hi2c->Instance->SR1);
			//wait(4);
		}

    }
	else if(((sr1itflags & I2C_FLAG_BTF) != RESET) && ((itsources & I2C_IT_EVT) != RESET))
	{
		REG_ADD(i2cpointer);
		REG_ADD(i2cpointer);
		//wait(5);
	}


}



#endif
