/*
 * app_typedef.h
 *
 *  Created on: May 17, 2018
 *      Author: kevinweiss
 */

#ifndef APP_TYPEDEF_H_
#define APP_TYPEDEF_H_

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "app_errno.h"


typedef struct i2c_mode_t_TAG
{
	#pragma pack(1)
	uint8_t addr_10_bit		: 1;
	uint8_t general_call	: 1;
	uint8_t no_clk_stretch	: 1;
}i2c_mode_t;

//Author: Kevin Weiss
//Revision: 1.00.00d
// Max structure size definitions
#define TIMESTAMP_T_SIZE 8
#define SYS_T_SIZE 32
#define I2C_T_SIZE 16
#define SPI_T_SIZE 16
#define UART_T_SIZE 16
#define ADC_T_SIZE 16
#define PWM_T_SIZE 16
#define TMR_T_SIZE 16
#define MAP_T_SIZE 256


// Array size definitions
#define SN_AMOUNT 12
#define ADC_AMOUNT 2


// Array offset definitions
#define TIMESTAMP_T_SECOND_OFFSET 0
#define TIMESTAMP_T_MINUTE_OFFSET 1
#define TIMESTAMP_T_HOUR_OFFSET 2
#define TIMESTAMP_T_DAY_OF_MONTH_OFFSET 3
#define TIMESTAMP_T_DAY_OF_WEEK_OFFSET 4
#define TIMESTAMP_T_MONTH_OFFSET 5
#define TIMESTAMP_T_YEAR_OFFSET 6
#define TIMESTAMP_T_RES_OFFSET 7

#define SYS_T_SN_OFFSET 0
#define SYS_T_FW_REV_OFFSET 12
#define SYS_T_BUILD_TIME_OFFSET 16
#define SYS_T_DEVICE_NUM_OFFSET 24
#define SYS_T_RES_OFFSET 28

#define I2C_T_MODE_OFFSET 0
#define I2C_T_ERROR_CODE_OFFSET 1
#define I2C_T_CLK_STRETCH_DELAY_OFFSET 3
#define I2C_T_INJECT_FAILURE_MODE_OFFSET 5
#define I2C_T_SLAVE_ADDR_1_OFFSET 6
#define I2C_T_SLAVE_ADDR_2_OFFSET 8
#define I2C_T_RES_OFFSET 10

#define SPI_T_MODE_OFFSET 0
#define SPI_T_ERROR_CODE_OFFSET 1
#define SPI_T_RES_OFFSET 5

#define UART_T_MODE_OFFSET 0
#define UART_T_ERROR_CODE_OFFSET 1
#define UART_T_BAUD_OFFSET 3
#define UART_T_REG_OUTPUT_OFFSET 7
#define UART_T_SIZE_OFFSET 8
#define UART_T_RES_OFFSET 9

#define ADC_T_MODE_OFFSET 0
#define ADC_T_ERROR_CODE_OFFSET 1
#define ADC_T_SAMPLE_RATE_OFFSET 3
#define ADC_T_VALUE_OFFSET 4
#define ADC_T_RES_OFFSET 8

#define PWM_T_MODE_OFFSET 0
#define PWM_T_ERROR_CODE_OFFSET 1
#define PWM_T_DUTY_OFFSET 3
#define PWM_T_FREQ_OFFSET 4
#define PWM_T_RES_OFFSET 8

#define TMR_T_MODE_OFFSET 0
#define TMR_T_ERROR_CODE_OFFSET 1
#define TMR_T_DUTY_OFFSET 3
#define TMR_T_FREQ_OFFSET 4
#define TMR_T_HI_US_OFFSET 8
#define TMR_T_LO_US_OFFSET 12

#define MAP_T_SYS_OFFSET 0
#define MAP_T_I2C_OFFSET 32
#define MAP_T_SPI_OFFSET 48
#define MAP_T_UART_OFFSET 64
#define MAP_T_RTC_OFFSET 80
#define MAP_T_ADC_OFFSET 88
#define MAP_T_PWM_OFFSET 120
#define MAP_T_TMR_OFFSET 136
#define MAP_T_RES_OFFSET 152



typedef union timestamp_t_TAG
{
	uint8_t data8[TIMESTAMP_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t second;
		uint8_t minute;
		uint8_t hour;
		uint8_t day_of_month;
		uint8_t day_of_week;
		uint8_t month;
		uint8_t year;
		uint8_t res[1];
	};
}timestamp_t;

typedef union sys_t_TAG
{
	uint8_t data8[SYS_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t sn[SN_AMOUNT];
		uint32_t fw_rev;
		timestamp_t build_time;
		uint32_t device_num;
		uint8_t res[4];
	};
}sys_t;

typedef union i2c_t_TAG
{
	uint8_t data8[I2C_T_SIZE];
	struct
	{
		#pragma pack(1)
		i2c_mode_t mode;
		uint16_t error_code;
		uint16_t clk_stretch_delay;
		uint8_t inject_failure_mode;
		uint16_t slave_addr_1;
		uint16_t slave_addr_2;
		uint8_t res[6];
	};
}i2c_t;

typedef union spi_t_TAG
{
	uint8_t data8[SPI_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t mode;
		uint32_t error_code;
		uint8_t res[11];
	};
}spi_t;

typedef union uart_t_TAG
{
	uint8_t data8[UART_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t mode;
		uint16_t error_code;
		uint32_t baud;
		uint8_t reg_output;
		uint8_t size;
		uint8_t res[7];
	};
}uart_t;

typedef union adc_t_TAG
{
	uint8_t data8[ADC_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t mode;
		uint16_t error_code;
		uint8_t sample_rate;
		uint32_t value;
		uint8_t res[8];
	};
}adc_t;

typedef union pwm_t_TAG
{
	uint8_t data8[PWM_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t mode;
		uint16_t error_code;
		uint8_t duty;
		uint32_t freq;
		uint8_t res[8];
	};
}pwm_t;

typedef union tmr_t_TAG
{
	uint8_t data8[TMR_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t mode;
		uint16_t error_code;
		uint8_t duty;
		uint32_t freq;
		uint32_t hi_us;
		uint32_t lo_us;
	};
}tmr_t;

typedef union map_t_TAG
{
	uint8_t data8[MAP_T_SIZE];
	struct
	{
		#pragma pack(1)
		sys_t sys;
		i2c_t i2c;
		spi_t spi;
		uart_t uart;
		timestamp_t rtc;
		adc_t adc[ADC_AMOUNT];
		pwm_t pwm;
		tmr_t tmr;
		uint8_t res[104];
	};
}map_t;

#endif /* APP_TYPEDEF_H_ */
