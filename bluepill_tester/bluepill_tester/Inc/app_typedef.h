/*
 * app_typedef.h
 *
 *  Created on: May 17, 2018
 *      Author: kevinweiss
 */

#ifndef APP_TYPEDEF_H_
#define APP_TYPEDEF_H_


typedef struct i2c_mode_t_TAG
{
	#pragma pack(1)
	uint8_t addr_10_bit		: 1;
	uint8_t general_call	: 1;
	uint8_t no_clk_stretch	: 1;
}i2c_mode_t;

//Author: Kevin Weiss
//Revision: 1.00.00c
// Max structure size definitions
#define TIMESTAMP_T_SIZE 8
#define SYS_T_SIZE 32
#define I2C_T_SIZE 16
#define SPI_T_SIZE 8
#define UART_T_SIZE 8
#define ADC_T_SIZE 8
#define PWM_T_SIZE 8
#define TMR_T_SIZE 8
#define MAP_T_SIZE 128


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
#define SYS_T_ECHO_OFFSET 24
#define SYS_T_ECHO_P1_OFFSET 25
#define SYS_T_CONST_TEST_OFFSET 26
#define SYS_T_RES_OFFSET 28

#define I2C_T_SLAVE_ADDR_1_OFFSET 0
#define I2C_T_SLAVE_ADDR_2_OFFSET 2
#define I2C_T_INCREMENT_MEM_OFFSET 4
#define I2C_T_ERROR_OFFSET 5
#define I2C_T_MODE_OFFSET 6
#define I2C_T_INJECT_FAILURE_MODE_OFFSET 7
#define I2C_T_DATA_OFFSET 8
#define I2C_T_RES_OFFSET 12

#define SPI_T_MODE_OFFSET 0
#define SPI_T_DATA_8_OFFSET 1
#define SPI_T_DATA_16_OFFSET 2
#define SPI_T_RES_OFFSET 4

#define UART_T_RES_OFFSET 0

#define ADC_T_RES_OFFSET 0

#define PWM_T_RES_OFFSET 0

#define TMR_T_RES_OFFSET 0

#define MAP_T_SYS_OFFSET 0
#define MAP_T_I2C_OFFSET 32
#define MAP_T_SPI_OFFSET 48
#define MAP_T_UART_OFFSET 56
#define MAP_T_RTC_OFFSET 64
#define MAP_T_ADC_OFFSET 72
#define MAP_T_PWM_OFFSET 88
#define MAP_T_TMR_OFFSET 96
#define MAP_T_RES_OFFSET 104



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
		uint8_t echo;
		uint8_t echo_p1;
		uint16_t const_test;
		uint8_t res[4];
	};
}sys_t;

typedef union i2c_t_TAG
{
	uint8_t data8[I2C_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint16_t slave_addr_1;
		uint16_t slave_addr_2;
		uint8_t increment_mem;
		uint8_t error;
		i2c_mode_t mode;
		uint8_t inject_failure_mode;
		uint32_t data;
		uint8_t res[4];
	};
}i2c_t;

typedef union spi_t_TAG
{
	uint8_t data8[SPI_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t mode;
		uint8_t data_8;
		uint16_t data_16;
		uint8_t res[4];
	};
}spi_t;

typedef union uart_t_TAG
{
	uint8_t data8[UART_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t res[8];
	};
}uart_t;

typedef union adc_t_TAG
{
	uint8_t data8[ADC_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t res[8];
	};
}adc_t;

typedef union pwm_t_TAG
{
	uint8_t data8[PWM_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t res[8];
	};
}pwm_t;

typedef union tmr_t_TAG
{
	uint8_t data8[TMR_T_SIZE];
	struct
	{
		#pragma pack(1)
		uint8_t res[8];
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
		uint8_t res[24];
	};
}map_t;

#endif /* APP_TYPEDEF_H_ */
