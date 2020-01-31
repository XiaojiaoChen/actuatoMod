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

#if I2C_HARDWARE==1
#include "i2c.h"
#else
#include "Wire.h"
#endif

static uint8_t IMUWriteBuffer[3];
static uint8_t IMUReadBuffer[8];

static struct QUATERNION quaternionOriginal;

void unpackQuaternion(struct QUATERNIONCOMPACT *qCom,struct QUATERNION *qOri);
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
	return IMU_ReadReg(IMU_WT902_Quaternion_Q0,(uint16_t *)quaternionBuf,4);
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




unsigned int fastSqrt32(unsigned long n)
{
    unsigned int c = 0x8000;
    unsigned int g = 0x8000;

    for(;;) {
        if(g*g > n)
            g ^= c;
        c >>= 1;
        if(c == 0)
            return g;
        g |= c;
    }
}

void packQuaternion(struct QUATERNION *qOri,struct QUATERNIONCOMPACT *qCom){
	uint16_t maxLoc=0;
	uint16_t *qOriInt16=(uint16_t *)qOri;
	for(int i=0;i<4;i++){
		if(i>0 && abs(qOriInt16[i])>abs(qOriInt16[maxLoc]))
			maxLoc=i;
	}
	if(maxLoc==0)
	{
		qCom->imuData0=(qOri->imuData1)>>2;
		qCom->imuData1=(qOri->imuData2)>>2;
		qCom->imuData2=(qOri->imuData3)>>2;
	}
	else if (maxLoc==1){
		qCom->imuData0=(qOri->imuData0)>>2;
		qCom->imuData1=(qOri->imuData2)>>2;
		qCom->imuData2=(qOri->imuData3)>>2;
	}
	else if(maxLoc==2){
		qCom->imuData0=(qOri->imuData0)>>2;
		qCom->imuData1=(qOri->imuData1)>>2;
		qCom->imuData2=(qOri->imuData3)>>2;
	}
	else{
		qCom->imuData0=(qOri->imuData0)>>2;
		qCom->imuData1=(qOri->imuData1)>>2;
		qCom->imuData2=(qOri->imuData2)>>2;
	}
	qCom->maxLocHigh=maxLoc>>1;
	qCom->maxLocLow=maxLoc;
	qCom->maxSign=((uint16_t)(((uint16_t)qOriInt16[maxLoc])&0x8000))>>15;
}

void unpackQuaternion(struct QUATERNIONCOMPACT *qCom,struct QUATERNION *qOri){
	int32_t lastNumOri=0;
	uint16_t maxLoc=(uint16_t)(qCom->maxLocHigh<<1 | qCom->maxLocLow);
	int16_t qRes0=(int16_t)(((uint16_t)qCom->imuData0)<<2);
	int16_t qRes1=(int16_t)(((uint16_t)qCom->imuData1)<<2);
	int16_t qRes2=(int16_t)(((uint16_t)qCom->imuData2)<<2);

	uint32_t sum=qRes0*qRes0+qRes1*qRes1+qRes2*qRes2;
	const int maxNum=(uint32_t)(1<<30);
	lastNumOri=(sum>=maxNum?0:fastSqrt32((uint32_t)(maxNum-sum)));
	int16_t lastNum=qCom->maxSign?-lastNumOri:lastNumOri;
	if(maxLoc==0)
	{
		qOri->imuData1=qRes0;
		qOri->imuData2=qRes1;
		qOri->imuData3=qRes2;
		qOri->imuData0=lastNum;
	}
	else if (maxLoc==1){
		qOri->imuData0=qRes0;
		qOri->imuData2=qRes1;
		qOri->imuData3=qRes2;
		qOri->imuData1=lastNum;
	}
	else if(maxLoc==2){
		qOri->imuData0=qRes0;
		qOri->imuData1=qRes1;
		qOri->imuData3=qRes2;
		qOri->imuData2=lastNum;
	}
	else{
		qOri->imuData0=qRes0;
		qOri->imuData1=qRes1;
		qOri->imuData2=qRes2;
		qOri->imuData3=lastNum;
	}
}
