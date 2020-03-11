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


 struct CANBUS_HANDLE{
	CAN_HandleTypeDef     CanHandle;
	CAN_TxHeaderTypeDef   TxHeader;
	CAN_RxHeaderTypeDef   RxHeader;
	uint8_t               *TxData;
	uint8_t               *RxData;
	uint32_t              TxMailbox;
 };

 extern struct CANBUS_HANDLE canbus;
 extern struct SENSORDATA sensorData;
 extern struct SENSORDATA sensorDataRx;
 extern int16_t imuOriData[];
extern int16_t imuGetData[];


extern uint32_t ADCBuffer;

void canConfig();
void readSensors();
uint8_t readIMU();
uint8_t readPressure();
uint8_t readLaser(uint16_t *);
void pressureADReadCallback();
void canSend();
#ifdef __cplusplus
}
#endif
#endif /* SENSOR_H_ */
