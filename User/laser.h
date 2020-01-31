/*
 * laser.h
 *
 *  Created on: Jan 19, 2020
 *      Author: Xiaojiao Chen
 *       Email: yii402072495@gmail.com
 */

#ifndef LASER_H_
#define LASER_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "vl6180x_api.h"


#define LASERI2CADDRESS 	  ((uint8_t)(0x29<<1))


void initLaserPoll();
uint8_t laserStartSingleShot();
uint8_t laserTryRead(uint16_t *);


void initLaserInt();
uint8_t readLaserInt(uint16_t *);
void VL6180x_IntCallback();

extern volatile int IntrFired;

extern VL6180x_RangeData_t RangeData;



#ifdef __cplusplus
}
#endif
#endif /* LASER_H_ */
