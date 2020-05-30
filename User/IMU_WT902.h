/*
 * IMU_WT902.h
 *
 *  Created on: Jan 13, 2020
 *      Author: Xiaojiao Chen
 *       Email: yii402072495@gmail.com
 */

#ifndef IMU_WT902_H_
#define IMU_WT902_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "messages.h"

#define IMU_WT902_SAVE 			0x00
#define IMU_WT902_CALSW 		0x01
#define IMU_WT902_RSW 			0x02
#define IMU_WT902_RRATE			0x03
#define IMU_WT902_BAUD 			0x04
#define IMU_WT902_AXOFFSET	0x05
#define IMU_WT902_AYOFFSET	0x06
#define IMU_WT902_AZOFFSET	0x07
#define IMU_WT902_GXOFFSET	0x08
#define IMU_WT902_GYOFFSET	0x09
#define IMU_WT902_GZOFFSET	0x0a
#define IMU_WT902_HXOFFSET	0x0b
#define IMU_WT902_HYOFFSET	0x0c
#define IMU_WT902_HZOFFSET	0x0d
#define IMU_WT902_D0MODE		0x0e
#define IMU_WT902_D1MODE		0x0f
#define IMU_WT902_D2MODE		0x10
#define IMU_WT902_D3MODE		0x11
#define IMU_WT902_D0PWMH		0x12
#define IMU_WT902_D1PWMH		0x13
#define IMU_WT902_D2PWMH		0x14
#define IMU_WT902_D3PWMH		0x15
#define IMU_WT902_D0PWMT		0x16
#define IMU_WT902_D1PWMT		0x17
#define IMU_WT902_D2PWMT		0x18
#define IMU_WT902_D3PWMT		0x19
#define IMU_WT902_IICADDR		0x1a
#define IMU_WT902_LEDOFF 		0x1b
#define IMU_WT902_GPSBAUD		0x1c

#define IMU_WT902_YYMM				0x30
#define IMU_WT902_DDHH				0x31
#define IMU_WT902_MMSS				0x32
#define IMU_WT902_MS					0x33
#define IMU_WT902_AX					0x34
#define IMU_WT902_AY					0x35
#define IMU_WT902_AZ					0x36
#define IMU_WT902_GX					0x37
#define IMU_WT902_GY					0x38
#define IMU_WT902_GZ					0x39
#define IMU_WT902_HX					0x3a
#define IMU_WT902_HY					0x3b
#define IMU_WT902_HZ					0x3c
#define IMU_WT902_Roll				0x3d
#define IMU_WT902_Pitch				0x3e
#define IMU_WT902_Yaw					0x3f
#define IMU_WT902_TEMP				0x40
#define IMU_WT902_D0Status		0x41
#define IMU_WT902_D1Status		0x42
#define IMU_WT902_D2Status		0x43
#define IMU_WT902_D3Status		0x44
#define IMU_WT902_PressureL		0x45
#define IMU_WT902_PressureH		0x46
#define IMU_WT902_HeightL			0x47
#define IMU_WT902_HeightH			0x48
#define IMU_WT902_LonL				0x49
#define IMU_WT902_LonH				0x4a
#define IMU_WT902_LatL				0x4b
#define IMU_WT902_LatH				0x4c
#define IMU_WT902_GPSHeight   0x4d
#define IMU_WT902_GPSYAW      0x4e
#define IMU_WT902_GPSVL				0x4f
#define IMU_WT902_GPSVH				0x50
#define IMU_WT902_Quaternion_Q0		0x51
#define IMU_WT902_Quaternion_Q1		0x52
#define IMU_WT902_Quaternion_Q2		0x53
#define IMU_WT902_Quaternion_Q3		0x54


#define IMU_WT902_DIO_MODE_AIN 0
#define IMU_WT902_DIO_MODE_DIN 1
#define IMU_WT902_DIO_MODE_DOH 2
#define IMU_WT902_DIO_MODE_DOL 3
#define IMU_WT902_DIO_MODE_DOPWM 4
#define IMU_WT902_DIO_MODE_GPS 5

#define IMU_I2C_ADDRESS         ((uint8_t)(0x50<<1))
#define LH2UINT16(x,y) ((uint16_t)((uint16_t)(x)|(uint16_t)((y)<<8)))

uint8_t IMU_Init();
uint8_t readIMU_Quaternions(QUATERNION *);
uint8_t readIMU_QuaternionsPacked(QUATERNIONCOMPACT *);

#ifdef __cplusplus
}
#endif
#endif /* IMU_WT902_H_ */
