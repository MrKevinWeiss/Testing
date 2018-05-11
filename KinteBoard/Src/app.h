/*
 * app.h
 *
 *  Created on: Apr 27, 2018
 *      Author: kevinweiss
 */

#ifndef APP_H_
#define APP_H_

void init_app();
void i2c_it(I2C_HandleTypeDef* hi2c);
void i2c_err(I2C_HandleTypeDef* hi2c);

#endif /* APP_H_ */
