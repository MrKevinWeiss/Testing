/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2018 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LED0_Pin GPIO_PIN_13
#define LED0_GPIO_Port GPIOC
#define DEBUG0_Pin GPIO_PIN_0
#define DEBUG0_GPIO_Port GPIOA
#define DEBUG1_Pin GPIO_PIN_1
#define DEBUG1_GPIO_Port GPIOA
#define DEBUG2_Pin GPIO_PIN_2
#define DEBUG2_GPIO_Port GPIOA
#define DEBUG3_Pin GPIO_PIN_3
#define DEBUG3_GPIO_Port GPIOA
#define TEST_FAIL_Pin GPIO_PIN_4
#define TEST_FAIL_GPIO_Port GPIOA
#define TEST_WARN_Pin GPIO_PIN_5
#define TEST_WARN_GPIO_Port GPIOA
#define TEST_PASS_Pin GPIO_PIN_6
#define TEST_PASS_GPIO_Port GPIOA
#define DUT_ADC_Pin GPIO_PIN_7
#define DUT_ADC_GPIO_Port GPIOA
#define PM_V_ADC_Pin GPIO_PIN_0
#define PM_V_ADC_GPIO_Port GPIOB
#define PM_A_ADC_Pin GPIO_PIN_1
#define PM_A_ADC_GPIO_Port GPIOB
#define DUT_TX_Pin GPIO_PIN_10
#define DUT_TX_GPIO_Port GPIOB
#define DUT_RX_Pin GPIO_PIN_11
#define DUT_RX_GPIO_Port GPIOB
#define DUT_RST_Pin GPIO_PIN_12
#define DUT_RST_GPIO_Port GPIOB
#define DUT_CTS_Pin GPIO_PIN_13
#define DUT_CTS_GPIO_Port GPIOB
#define DUT_RTS_Pin GPIO_PIN_14
#define DUT_RTS_GPIO_Port GPIOB
#define USER_BTN_Pin GPIO_PIN_15
#define USER_BTN_GPIO_Port GPIOB
#define DUT_IC_Pin GPIO_PIN_8
#define DUT_IC_GPIO_Port GPIOA
#define IF_TX_Pin GPIO_PIN_9
#define IF_TX_GPIO_Port GPIOA
#define IF_RX_Pin GPIO_PIN_10
#define IF_RX_GPIO_Port GPIOA
#define DUT_NSS_Pin GPIO_PIN_15
#define DUT_NSS_GPIO_Port GPIOA
#define DUT_SCK_Pin GPIO_PIN_3
#define DUT_SCK_GPIO_Port GPIOB
#define DUT_MISO_Pin GPIO_PIN_4
#define DUT_MISO_GPIO_Port GPIOB
#define DUT_MOSI_Pin GPIO_PIN_5
#define DUT_MOSI_GPIO_Port GPIOB
#define DUT_SCL_Pin GPIO_PIN_6
#define DUT_SCL_GPIO_Port GPIOB
#define DUT_SDA_Pin GPIO_PIN_7
#define DUT_SDA_GPIO_Port GPIOB
#define DUT_DAC_Pin GPIO_PIN_8
#define DUT_DAC_GPIO_Port GPIOB
#define DUT_PWM_Pin GPIO_PIN_9
#define DUT_PWM_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
 * @brief Uncomment the line below to expanse the "assert_param" macro in the
 *        HAL drivers code
 */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
