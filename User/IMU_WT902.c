/*
 * IMU_WT902.c
 *
 *  Created on: Jan 13, 2020
 *      Author: Xiaojiao Chen
 *       Email: yii402072495@gmail.com
 */
#include <stm32f1xx_hal.h>
#include "main.h"
#include "IMU_WT902.h"
#include "stdio.h"
#include "stdlib.h"
#include "messages.h"
#if I2C_HARDWARE==1
#include "i2c.h"
#else
#include "Wire.h"
#endif

static uint8_t IMUWriteBuffer[3];
static uint8_t IMUReadBuffer[8];

static struct QUATERNION quaternionOriginal;

static uint8_t IMU_WriteReg(uint8_t regAdd,uint16_t val){
	int ret=0;
	IMUWriteBuffer[0]=regAdd;
	IMUWriteBuffer[1]=(uint8_t)(val&0x00FF);
	IMUWriteBuffer[2]=(uint8_t)(val>>8);
	if ((ret=HAL_I2C_Master_Transmit(&hi2c2,IMU_I2C_ADDRESS,IMUWriteBuffer,3,1))!=HAL_OK){
		printf("IMU Write Reg %d , Error Code=%d\r\n",regAdd,ret);
	}
	return ret;
}
static uint8_t IMU_ReadReg(uint8_t regAdd,uint16_t *readBuf,uint16_t regNum){
	int ret;
	if (regNum>4)
		regNum=4;
	else if(regNum<=0)
		regNum=1;
	if ((ret=HAL_I2C_Mem_Read(&hi2c2,IMU_I2C_ADDRESS,regAdd,I2C_MEMADD_SIZE_8BIT,(uint8_t *)IMUReadBuffer,regNum*2,regNum))==HAL_OK){
		for(int i=0;i<regNum;i++){
			readBuf[i]=LH2UINT16(IMUReadBuffer[2*i],IMUReadBuffer[2*i+1]);
		}
	}
	else
		printf("IMU Read Reg %d , Error Code=%d\r\n",regAdd,ret);
	return ret;
}



uint8_t readIMU_Quaternions(struct QUATERNION *quaternionBuf)
{
	return IMU_ReadReg(IMU_WT902_Quaternion_Q0,(uint16_t *)quaternionBuf->imuData,4);
}

uint8_t readIMU_QuaternionsPacked( struct QUATERNIONCOMPACT *quaternionPactBuf)
{
	uint8_t ret;
	if((ret=readIMU_Quaternions(&quaternionOriginal))==HAL_OK)
		packQuaternion(&quaternionOriginal,quaternionPactBuf);
	return ret;
}

uint8_t IMU_Init()
{
	uint8_t ret;
	ret=IMU_WriteReg(IMU_WT902_RSW,0x031E);
	return ret;
}



