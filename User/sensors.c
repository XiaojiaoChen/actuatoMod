/*
 * Sensors.c
 *
 *  Created on: Oct 10, 2019
 *      Author: Xiaojiao Chen
 *       Email: yii402072495@gmail.com
 */

#include "main.h"
#include "sensors.h"
#include "messages.h"
#include "stdio.h"
#include "kalman.h"
#include "stdlib.h"

#if I2C_HARDWARE==1
#include "i2c.h"
#else
#include "Wire.h"
#endif


#define CANBUS_ID 	0


#define PRESSUREI2CADDRESS    ((uint8_t)(0x28<<1))


struct SENSORDATA sensorData;
struct SENSORDATA sensorDataRx;

struct CANBUS_HANDLE canbus;
struct CANBUS_HANDLE *ptcanbus=&canbus;

uint8_t PressureBuffer[2];
int16_t pressureRaw;
uint32_t pressureGlobal;

int16_t imuOriData[4];
int16_t imuGetData[4];





uint16_t laserDis;

int32_t elapTime=0;
float testDistance[2]={0,0};
//#define NUM_TAPS  2
//#define BLOCK_SIZE 1
//const float32_t firCoeffs32[NUM_TAPS] = {   0.93 ,0.07};
//static arm_fir_instance_f32 S;
//static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];
//float orirange=0;
//float firrange=0;


uint16_t laserDis;
void readSensors()
{
	uint8_t ret[3]={0,0,0};

 	ret[0]=readPressure();

	ret[1]=readIMU();
//
	ret[2]=readLaser(&laserDis);
	sensorData.distance=laserDis;


//	if(ret[0]||ret[1]||ret[2])
//	{
//		HAL_I2C_ErrorCallback(&hi2c2);
//	}
}


/***************************    Pressure Sensor      ********************/
/***************************                          *******************/
uint8_t readPressure()
{
	uint8_t ret;
	if((ret=HAL_I2C_Master_Receive(&hi2c2, PRESSUREI2CADDRESS, PressureBuffer, 2,1))==HAL_OK){
		pressureRaw = (int16_t) (((PressureBuffer[0]<<8) | PressureBuffer[1])&0x3FFF);

#if COMPACT_VERSION_PRESSURE_HPA == 1
	sensorData.pressure=(((uint32_t)((pressureRaw-1638)*30.51804f))>>20); //30.51804f=0.0000762951f*400000
#else
	sensorData.pressure=(uint16_t)((pressureRaw-1638)*0.0000762951f*400);
#endif

	}
	else
	{
		printf("pressure err %d\r\n",ret);

	}
	return ret;
}


/***************************     IMU Wt902			 ********************/
/***************************                          *******************/
uint8_t readIMU(){
	static uint32_t imuInitFlag=0;
	uint8_t ret;
	if(!imuInitFlag){
		if((ret=IMU_Init())==HAL_OK)
			imuInitFlag=1;
	}
	if((ret=readIMU_QuaternionsPacked(&(sensorData.quaternionCom))!=HAL_OK))
	{
		printf("IMU err\r\n");
	}
	return ret;
}




/****************Laser Vl6180x*******************/
uint8_t readLaser(uint16_t *buf){
	uint8_t ret;
#if LASER_MODE_POLL==1
	static uint8_t laserInitPollflag = 0;
	if (!laserInitPollflag) {
		initLaserPoll();
		laserInitPollflag = 1;
	}

	ret=laserTryRead(buf);

#else
	static uint8_t laserInitIntflag = 0;
		if (!laserInitIntflag) {
			initLaserInt();
			laserInitIntflag = 1;
		}
		ret=laserDis=readLaserInt(buf);
#endif

	sensorData.distance=*buf;

	/***Pack laser Data***/
#if COMPACT_VERSION_PRESSURE_HPA == 1
	sensorData.distance=(*buf)>>3;
	sensorData.quaternionCom.distanceBit2=(uint8_t)((*buf)>>2)&0x0001;
	sensorData.quaternionCom.distanceBit1=(uint8_t)((*buf)>>1)&0x0001;
	sensorData.quaternionCom.distanceBit0=(uint8_t)(*buf)&0x0001;
#else
	sensorData.distance=*buf;
#endif

	laserStartSingleShot();
	return ret;
}





/***************************     Can bus        ***************************/
/***************************                     ***************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
void canConfig(){
	canbus.CanHandle=hcan;
	CAN_FilterTypeDef  sFilterConfig;
	/*##-2- Configure the CAN Filter ###########################################*/
	/*For single CAN instance(14 dedicated filter banks)*/

	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&canbus.CanHandle, &sFilterConfig) != HAL_OK)
	{
	/* Filter configuration Error */
	Error_Handler();
	}

	/*##-3- Start the CAN peripheral ###########################################*/
	if (HAL_CAN_Start(&canbus.CanHandle) != HAL_OK)
	{
	/* Start Error */
	Error_Handler();
	}

	/*##-4- Activate CAN RX notification #######################################*/
	if (HAL_CAN_ActivateNotification(&canbus.CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING|CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
	{
	/* Notification Error */
	Error_Handler();
	}


}

void canSend()
{
	/*##-Configure Transmission Packet #####################################*/
	canbus.TxHeader.StdId = CANBUS_ID;
	canbus.TxHeader.ExtId = 0x01;
	canbus.TxHeader.RTR = CAN_RTR_DATA;
	canbus.TxHeader.IDE = CAN_ID_STD;
	canbus.TxHeader.TransmitGlobalTime = DISABLE;

	canbus.TxHeader.DLC = sizeof(struct SENSORDATA);
	canbus.TxData=(uint8_t *)(&sensorData);


	/* Start the Transmission process */
	if (HAL_CAN_AddTxMessage(&canbus.CanHandle, &canbus.TxHeader, canbus.TxData, &canbus.TxMailbox) != HAL_OK)
	{
	  /* Transmission request Error */
	  Error_Handler();
	}
}
void canRead(){
	 unpackQuaternion(&(sensorDataRx.quaternionCom), (struct QUATERNION*)imuGetData);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  canbus.RxData=(uint8_t *)(&sensorDataRx);
  if (HAL_CAN_GetRxMessage(&canbus.CanHandle, CAN_RX_FIFO0, &canbus.RxHeader, canbus.RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
  canRead();

}











#if I2C_HARDWARE == 1
static void wait_for_gpio_state_timeout_us(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state, uint32_t timeout)
 {
    while(state != HAL_GPIO_ReadPin(port, pin) && (timeout--)){
    	delay_us(1);
    }
}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	uint32_t timeout=40;//us
    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
	if(hi2c==&hi2c2){
	uint32_t SDA_PIN = GPIO_PIN_11;
	uint32_t SCL_PIN = GPIO_PIN_10;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. Clear PE bit.
    __HAL_I2C_DISABLE(hi2c);

    //  2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
 //   HAL_I2C_DeInit(hi2c);

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    GPIO_InitStructure.Pin = SCL_PIN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = SDA_PIN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 3. Check SCL and SDA High level in GPIOx_IDR.
    HAL_GPIO_WritePin(GPIOB, SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, SCL_PIN, GPIO_PIN_SET);

    wait_for_gpio_state_timeout_us(GPIOB, SCL_PIN, GPIO_PIN_SET, timeout);
    wait_for_gpio_state_timeout_us(GPIOB, SDA_PIN, GPIO_PIN_SET, timeout);

    // 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
    HAL_GPIO_WritePin(GPIOB, SDA_PIN, GPIO_PIN_RESET);

    // 5. Check SDA Low level in GPIOx_IDR.
    wait_for_gpio_state_timeout_us(GPIOB, SDA_PIN, GPIO_PIN_RESET, timeout);

    // 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
    HAL_GPIO_WritePin(GPIOB, SCL_PIN, GPIO_PIN_RESET);

    // 7. Check SCL Low level in GPIOx_IDR.
    wait_for_gpio_state_timeout_us(GPIOB, SCL_PIN, GPIO_PIN_RESET, timeout);

    // 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
    HAL_GPIO_WritePin(GPIOB, SCL_PIN, GPIO_PIN_SET);

    // 9. Check SCL High level in GPIOx_IDR.
    wait_for_gpio_state_timeout_us(GPIOB, SCL_PIN, GPIO_PIN_SET, timeout);

    // 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
    HAL_GPIO_WritePin(GPIOB, SDA_PIN, GPIO_PIN_SET);

    // 11. Check SDA High level in GPIOx_IDR.
    wait_for_gpio_state_timeout_us(GPIOB, SDA_PIN, GPIO_PIN_SET, timeout);

    // 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
    GPIO_InitStructure.Pin = SDA_PIN|SCL_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 13. Set SWRST bit in I2Cx_CR1 register.
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_SWRST);
    asm("nop");

    /* 14. Clear SWRST bit in I2Cx_CR1 register. */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_SWRST);
    asm("nop");

    /* 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register */
    __HAL_I2C_ENABLE(hi2c);
    asm("nop");

    // Call initialization function.
    MX_I2C2_Init();
	}
}
#endif
