/*
 * sensor.h
 *
 *  Created on: Oct 10, 2019
 *      Author: Xiaojiao Chen
 *       Email: yii402072495@gmail.com
 */

#ifndef SENSORS_H_
#define SENSORS_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include <stm32f1xx_hal.h>
#include "laser.h"
#include "can.h"
#include "IMU_WT902.h"
#include "messages.h"


 typedef struct CANBUS_HANDLE_TAG{
	CAN_HandleTypeDef     CanHandle;
	CAN_TxHeaderTypeDef   TxHeader;
	CAN_RxHeaderTypeDef   RxHeader;
	uint8_t               *TxData;
	uint8_t               *RxData;
	uint32_t              TxMailbox;
 }CANBUS_HANDLE;


 extern CANBUS_HANDLE canbus;
 extern SENSORDATACOMPACT sensorData;
 extern SENSORDATACOMPACT sensorDataRx;


extern uint32_t ADCBuffer;

void canConfig();
void tryReadSensors();
uint8_t readIMU();
uint8_t readPressure();
uint8_t readLaser();
uint8_t readLaserTo(uint16_t *);
void pressureADReadCallback();
void canSend();
uint8_t initIMU();
uint8_t my_I2C_CheckError(I2C_HandleTypeDef *hi2c);
#ifdef __cplusplus
}
#endif
#endif /* SENSOR_H_ */
