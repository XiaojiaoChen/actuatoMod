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

#define CANBUS_ID 	32

#define PRESSUREI2CADDRESS    ((uint8_t)(0x28<<1))

SENSORDATACOMPACT sensorData;
SENSORDATACOMPACT sensorDataRx;

uint8_t canBusReceive[8];      //receive data from stm32 nucleo board

CANBUS_HANDLE canbus;
CANBUS_HANDLE *ptcanbus = &canbus;

uint8_t PressureBuffer[2];
uint16_t pressureRaw;
uint32_t pressureGlobal;

int16_t imuOriData[4];
int16_t imuGetData[4];

uint16_t laserDis;

int32_t elapTime = 0;
float testDistance[2] = { 0, 0 };
//#define NUM_TAPS  2
//#define BLOCK_SIZE 1
//const float32_t firCoeffs32[NUM_TAPS] = {   0.93 ,0.07};
//static arm_fir_instance_f32 S;
//static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];
//float orirange=0;
//float firrange=0;

uint16_t laserDis;

int sensorRet[3]={0,0,0};
int sensorErr[3]={0,0,0};
typedef uint8_t (*SENSOR_READ_FUNCS) (void);
SENSOR_READ_FUNCS sensorReadFunc[3]={readPressure,readIMU,readLaser};



void tryReadSensors() {

	uint8_t i2cTobeCorrectted=0;

	//For any live sensor who has an i2c issure, we mark a need to recover i2c, and minus the recovery chances of the sensor
	for(int i=0;i<3;i++){
		if(sensorErr[i]<10){
			if(sensorRet[i]!=HAL_OK){
				sensorErr[i]++;
			}
			else{
				sensorErr[i]=0;
			}
		}
		i2cTobeCorrectted+=sensorErr[i];
	}
	//If i2c is marked to be recovered, do it.
	if(i2cTobeCorrectted){
		HAL_I2C_ErrorCallback(&hi2c2);
	}

	for(int i=0;i<3;i++){
		if(sensorErr[i]<10){
			sensorRet[i]=sensorReadFunc[i]();
		}
	}
}

/***************************    Pressure Sensor      ********************/
/***************************                          *******************/
/*
 * Outputmax = 14745 counts (90% of 214 counts or 0x3999)
 * Outputmin = 1638 counts (10% of 214 counts or 0x0666)
 * Pressuremax = 0 psi
 * Pressuremin = 60 psi
 * PressureType=Absolute
 * 1 psi=6894.76 Pa
*/

uint8_t readPressure() {

	uint8_t ret=0;
#if COMPACT_VERSION_PRESSURE_HPA == 1
	static float pressureCoef=60*6894.76f/(14745-1638)/100;
#else
	static float pressureCoef=60*6894.76f/(14745-1638)/1000;
#endif

	if ((ret = HAL_I2C_Master_Receive(&hi2c2, PRESSUREI2CADDRESS,PressureBuffer, 2, 1)) == HAL_OK) {
		pressureRaw = (uint16_t) (((((uint16_t) PressureBuffer[0]) << 8)| PressureBuffer[1]) & 0x3FFF);
		sensorData.pressure = (uint16_t)((pressureRaw - 1638) * pressureCoef);//absolute pressure, set to offset

	}
	return ret;
}

/***************************     IMU Wt902			 ********************/
/***************************                          *******************/
uint8_t readIMU() {
	static uint32_t imuInitFlag = 0;
	uint8_t ret;
	if (!imuInitFlag) {
		if ((ret = IMU_Init()) == HAL_OK)
			imuInitFlag = 1;
	}
	ret = readIMU_QuaternionsPacked(&(sensorData.quaternionCom));

	return ret;
}

uint8_t readLaser(){
	uint8_t ret=0;
	ret=readLaserTo(&laserDis);
	return ret;
}

/****************Laser Vl6180x*******************/
uint8_t readLaserTo(uint16_t *buf) {
	uint8_t ret;
#if LASER_MODE_POLL==1
	static uint32_t datanotready = 0;
	static uint32_t laserInternalErr=0;
	static uint8_t laserInitPollPend = 1;
	if (laserInitPollPend) {
		initLaserPoll();
		laserInitPollPend = 0;
	}

	ret = laserTryRead(buf);


#else
	static uint8_t laserInitIntPend = 1;
		if (laserInitIntflag) {
			initLaserInt();
			laserInitIntPend = 0;
		}
		ret=laserDis=readLaserInt(buf);
#endif

	sensorData.distance = *buf;

	/***Pack laser Data***/
#if COMPACT_VERSION_PRESSURE_HPA == 1
	sensorData.distance = (*buf) >> 3;
	sensorData.quaternionCom.distanceBit2 = (uint8_t) ((*buf) >> 2) & 0x0001;
	sensorData.quaternionCom.distanceBit1 = (uint8_t) ((*buf) >> 1) & 0x0001;
	sensorData.quaternionCom.distanceBit0 = (uint8_t) (*buf) & 0x0001;
#else
	sensorData.distance = *buf;
#endif

	if (RangeData.errorStatus == NoError) {
		laserStartSingleShot();
	} else if(RangeData.errorStatus <=5){
		if (laserInternalErr++ > 50) {
			laserInitPollPend=1;
			laserInternalErr=0;
		}
	} else if(RangeData.errorStatus ==DataNotReady){
		if (datanotready++ > 50) {
			laserInitPollPend=1;
			datanotready=0;
		}
	}
	return ret;
}

/***************************     Can bus        ***************************/
/***************************                     ***************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
static void canStandBy(uint8_t canstand) {
	HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, canstand);
}

void canConfig() {
	canStandBy(0);

	canbus.CanHandle = hcan;
	CAN_FilterTypeDef sFilterConfig;
	/*##-2- Configure the CAN Filter ###########################################*/
	/*For single CAN instance(14 dedicated filter banks)*/

	/***********Receiving only from stm32 nucelo board(id = 0x400)**************/
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	sFilterConfig.FilterIdHigh = (uint16_t) ((uint16_t) (0x400)) << 5;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; //CAN_RX_FIFO0=0; CAN_RX_FIFO1=1
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	if (HAL_CAN_ConfigFilter(&canbus.CanHandle, &sFilterConfig) != HAL_OK) {
		/* Filter configuration Error */
		Error_Handler();
	}

	/*##-3- Start the CAN peripheral ###########################################*/
	if (HAL_CAN_Start(&canbus.CanHandle) != HAL_OK) {
		/* Start Error */
		Error_Handler();
	}

	/*##-4- Activate CAN RX notification #######################################*/
	if (HAL_CAN_ActivateNotification(&canbus.CanHandle,
			CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING)
			!= HAL_OK) {
		/* Notification Error */
		Error_Handler();
	}

}

void canSend() {
	/*##-Configure Transmission Packet #####################################*/
	canbus.TxHeader.StdId = CANBUS_ID;
	canbus.TxHeader.ExtId = 0x01;
	canbus.TxHeader.RTR = CAN_RTR_DATA;
	canbus.TxHeader.IDE = CAN_ID_STD;
	canbus.TxHeader.TransmitGlobalTime = DISABLE;

	canbus.TxHeader.DLC = sizeof(SENSORDATA);
	canbus.TxData = (uint8_t*) (&sensorData);

	/* Start the Transmission process */
	if (HAL_CAN_AddTxMessage(&canbus.CanHandle, &canbus.TxHeader, canbus.TxData,
			&canbus.TxMailbox) != HAL_OK) {
		/* Transmission request Error */
		Error_Handler();
	}
}

void HandleCANbusReceive() {
	//TODO:handle command pQ, pR, dt, kalman filter for laser
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	/* Get RX message */
	//canbus.RxData=(uint8_t *)(&sensorDataRx);
	canbus.RxData = (uint8_t*) (canBusReceive);

	if (HAL_CAN_GetRxMessage(&canbus.CanHandle, CAN_RX_FIFO0, &canbus.RxHeader,
			canbus.RxData) != HAL_OK) {
		/* Reception Error */
		Error_Handler();
	}

	//handle command pQ, pR, dt, kalman filter for laser
	HandleCANbusReceive();

	// pressure-offset
}

/**********************************************            ******************************************/
/**********************************************I2C handding******************************************/
/**********************************************            ******************************************/

#if I2C_HARDWARE == 1
static void wait_for_gpio_state_timeout_us(GPIO_TypeDef *port, uint16_t pin,
		GPIO_PinState state, uint32_t timeout) {
	while (state != HAL_GPIO_ReadPin(port, pin) && (timeout--)) {
		delay_us(1);
	}
}

uint8_t my_I2C_CheckError(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance->SR1 | hi2c->Instance->SR2)
		return 1;
	else
		return 0;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {

	uint32_t timeout = 40;  //us
	/**I2C2 GPIO Configuration
	 PB10     ------> I2C2_SCL
	 PB11     ------> I2C2_SDA
	 */
	if (my_I2C_CheckError(&hi2c2)) {
		if (hi2c == &hi2c2) {
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

			wait_for_gpio_state_timeout_us(GPIOB, SCL_PIN, GPIO_PIN_SET,
					timeout);
			wait_for_gpio_state_timeout_us(GPIOB, SDA_PIN, GPIO_PIN_SET,
					timeout);

			//3.5 still busy, may be the slave blocking. send 9 clocks and a stop.
//			if(HAL_GPIO_ReadPin(GPIOB, SDA_PIN)== GPIO_PIN_RESET){
//				//repeat three times.
//				for(int i=0;i<3;i++){
//					uint8_t clockBit=0;
//					while(HAL_GPIO_ReadPin(GPIOB, SDA_PIN)== GPIO_PIN_RESET && clockBit<9){
//						HAL_GPIO_WritePin(GPIOB, SCL_PIN, GPIO_PIN_RESET);
//						delay_us(500);
//						HAL_GPIO_WritePin(GPIOB, SCL_PIN, GPIO_PIN_SET);
//						delay_us(500);
//						clockBit++;
//					}
//					//generate a stop
//					HAL_GPIO_WritePin(GPIOB, SDA_PIN, GPIO_PIN_SET);
//					wait_for_gpio_state_timeout_us(GPIOB, SCL_PIN, GPIO_PIN_SET,
//										timeout);
//
//					//If successful then break the loop
//					if(HAL_GPIO_ReadPin(GPIOB, SDA_PIN)== GPIO_PIN_SET)
//						break;
//				}
//			}
			//I2c internal problem

			// 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
			HAL_GPIO_WritePin(GPIOB, SDA_PIN, GPIO_PIN_RESET);

			// 5. Check SDA Low level in GPIOx_IDR.
			wait_for_gpio_state_timeout_us(GPIOB, SDA_PIN, GPIO_PIN_RESET,
					timeout);

			// 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
			HAL_GPIO_WritePin(GPIOB, SCL_PIN, GPIO_PIN_RESET);

			// 7. Check SCL Low level in GPIOx_IDR.
			wait_for_gpio_state_timeout_us(GPIOB, SCL_PIN, GPIO_PIN_RESET,
					timeout);

			// 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
			HAL_GPIO_WritePin(GPIOB, SCL_PIN, GPIO_PIN_SET);

			// 9. Check SCL High level in GPIOx_IDR.
			wait_for_gpio_state_timeout_us(GPIOB, SCL_PIN, GPIO_PIN_SET,
					timeout);

			// 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
			HAL_GPIO_WritePin(GPIOB, SDA_PIN, GPIO_PIN_SET);

			// 11. Check SDA High level in GPIOx_IDR.
			wait_for_gpio_state_timeout_us(GPIOB, SDA_PIN, GPIO_PIN_SET,
					timeout);

			// 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
			GPIO_InitStructure.Pin = SDA_PIN | SCL_PIN;
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


			printf("I2c recovered\r\n");
		}
	}
}
#endif
