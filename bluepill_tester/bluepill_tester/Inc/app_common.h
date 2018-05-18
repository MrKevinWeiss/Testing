/*
 * app_common.h
 *
 *  Created on: May 17, 2018
 *      Author: kevinweiss
 */

#ifndef APP_COMMON_H_
#define APP_COMMON_H_

/*
 * @brief	Enables interrupts.
 *
 */
#define EN_INT __enable_irq()

/*
 * @brief	Disables interrupts.
 *
 */
#define DIS_INT __disable_irq()

/*
 * @brief	Disables interrupts.
 *
 */
#define SOFT_RESET NVIC_SystemReset()


void delay_us(uint16_t micros);

#endif /* APP_COMMON_H_ */
