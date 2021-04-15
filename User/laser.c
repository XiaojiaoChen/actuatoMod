/*
 * laser.c
 *
 *  Created on: Jan 19, 2020
 *      Author: Xiaojiao Chen
 *       Email: yii402072495@gmail.com
 */

#include "laser.h"
#include "kalman.h"
#include "stdio.h"
#include "usart.h"
#if I2C_HARDWARE==1
#include "i2c.h"
#else
#include "Wire.h"
#endif

volatile int IntrFired=0;
static int32_t lastResponseTime = 0;
static float filteredrangeFloat=0;
int range=0;
static uint16_t filteredrangeInt=0;

VL6180x_RangeData_t RangeData;
static VL6180xDev_t theVL6180xDev;
static KALMAN_FILTER *ptLaserKalman;


/***************************     VL6180x Laser Sensor ********************/
/***************************                          *******************/

void initLaserPoll() {
//	arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32[0], &firStateF32[0], BLOCK_SIZE);

	ptLaserKalman = KALMANFILTER(400, 12, 0.02);
	//ptLaserKalman = KALMANFILTER(pQ_set, pR_set, dt_set);

	theVL6180xDev = LASERI2CADDRESS;
	HAL_GPIO_WritePin(LASER_D1_GPIO_Port, LASER_D1_Pin, 0);
	HAL_Delay(3);
	HAL_GPIO_WritePin(LASER_D1_GPIO_Port, LASER_D1_Pin, 1);
	HAL_Delay(3);
	VL6180x_InitData(theVL6180xDev);
	VL6180x_FilterSetState(theVL6180xDev, 0); //disable filtering as not effective in continuous mode
	VL6180x_Prepare(theVL6180xDev);     // default vl6180x init
	VL6180x_SetupGPIO1(theVL6180xDev, CONFIG_GPIO_INTERRUPT_DISABLED,
			INTR_POL_HIGH);
	VL6180x_UpscaleSetScaling(theVL6180xDev, 1);
	VL6180x_RangeSetMaxConvergenceTime(theVL6180xDev, 20);
	VL6180x_RangeClearInterrupt(theVL6180xDev); // make sure no interrupt is pending

	laserStartSingleShot();

}

uint8_t laserStartSingleShot()
{
	return VL6180x_RangeStartSingleShot(theVL6180xDev);
}

uint8_t laserTryRead(uint16_t *buf) {
	uint8_t ret;

	if ((ret = VL6180x_RangeGetMeasurementIfReady(theVL6180xDev, &RangeData))== HAL_OK) {
		// Application must check Range.errorStatus before accessing the other data
		//    If Range.errorStatus is DataNotReady, application knows that it has to wait a bit before getting a new data
		//    If Range.errorStatus is 0, application knows it is a valid distance
		//    If Range.errorStatus is not 0, application knows that reported distance is invalid so may take some decisions depending on the errorStatus
		if (RangeData.errorStatus == DataNotReady)
		{
			return ret;
		}
		if (RangeData.errorStatus == 0) {
			//Option 1:Original Low pass, nearly same effect as the kalman filter
			int  LaserAlpha=0.83*(1<<16);
			range =  (range*LaserAlpha +  RangeData.range_mm * ((1<<16) - LaserAlpha))>>16;
			*buf =range;


			//Option 2:kalman filter
//			filteredrangeFloat = stepKF(ptLaserKalman, RangeData.range_mm);
//			*buf = (int) (round(filteredrangeFloat));

			//Option 3:FIR
//			orirange=RangeData.range_mm;
//			arm_fir_f32(&S, &(orirange), &(firrange), BLOCK_SIZE);
		}else
		{
			char* sc=VL6180x_RangeGetStatusErrString(RangeData.errorStatus);
			HAL_UART_Transmit(&huart1, (uint8_t *)sc, strlen(sc), 10);
		}
		/*  clear range interrupt source */
		VL6180x_RangeClearInterrupt(theVL6180xDev);
	}

	return ret;
}









void initLaserInt() {
	ptLaserKalman = KALMANFILTER(400, 12, 0.02);
	//ptLaserKalman = KALMANFILTER(pQ_set, pR_set, dt_set);

	theVL6180xDev = LASERI2CADDRESS;
	HAL_GPIO_WritePin(LASER_D1_GPIO_Port, LASER_D1_Pin, 0);
	HAL_Delay(7);
	HAL_GPIO_WritePin(LASER_D1_GPIO_Port, LASER_D1_Pin, 1);
	HAL_Delay(7);
	VL6180x_InitData(theVL6180xDev);
	VL6180x_FilterSetState(theVL6180xDev, 0); //disable filering as not effective in continuous mode
	VL6180x_Prepare(theVL6180xDev);     // default vl6180x init
	//	// if slow reaction is enough then set a high time like 100 ms (up to 2550 msec)
	//	// if fastest reaction is required then set 0  that will set minimal possible
	VL6180x_RangeSetInterMeasPeriod(theVL6180xDev, 0);
	//	// set vl6180x gpio1 pin to range interrupt output with high polarity (rising edge)
	VL6180x_SetupGPIO1(theVL6180xDev, GPIOx_SELECT_GPIO_INTERRUPT_OUTPUT,
			INTR_POL_HIGH);
	//	/* go ot new sampel ready */
	VL6180x_SetGroupParamHold(theVL6180xDev, 1);
	VL6180x_RangeConfigInterrupt(theVL6180xDev,
			CONFIG_GPIO_INTERRUPT_NEW_SAMPLE_READY);
	VL6180x_SetGroupParamHold(theVL6180xDev, 0);
	VL6180x_RangeClearInterrupt(theVL6180xDev); // clear flags
	VL6180x_RangeStartContinuousMode(theVL6180xDev);
	VL6180x_RangeSetMaxConvergenceTime(theVL6180xDev, 15);
	VL6180x_RangeClearInterrupt(theVL6180xDev); // make sure no interrupt is pending
	/* kick off the first measurement */
	//VL6180x_RangeStartSingleShot(theVL6180xDev);
}

uint8_t readLaserInt(uint16_t *buf) {

	int ret = 0;
	if (IntrFired) {
		IntrFired = 0;
		lastResponseTime = HAL_GetTick();
		if ((ret = VL6180x_RangeGetMeasurement(theVL6180xDev, &RangeData))
				== HAL_OK) {
			if (RangeData.errorStatus == 0) {
				//Option 1:Original Low pass, nearly same effect as the kalman filter
	//			int  LaserAlpha=0.83*(1<<16);
	//			range =  (range*LaserAlpha +  RangeData.range_mm * ((1<<16) - LaserAlpha))>>16;
	//			filteredrange =range;

				//Option 2:kalman filter
				filteredrangeFloat = stepKF(ptLaserKalman, RangeData.range_mm);
				*buf = (int) (round(filteredrangeFloat));

				//Option 3:FIR
	//			orirange=RangeData.range_mm;
	//			arm_fir_f32(&S, &(orirange), &(firrange), BLOCK_SIZE);
			}
			VL6180x_RangeClearInterrupt(theVL6180xDev);
		} else {
			printf("LaserError:%d\r\n", ret);
		}
	} else {
		if (((int32_t) HAL_GetTick() - lastResponseTime) > 100) {
			printf("laserTimeout\r\n,Re-arm");
			initLaserInt();
		}

	}
	return ret;
}

void VL6180x_IntCallback() {
	IntrFired = 1;

}











void VL6180x_PollDelay(VL6180xDev_t dev) {
	HAL_Delay(1);
}

/*Interface Functions to be implemented*/
int VL6180x_I2CWrite(VL6180xDev_t addr, uint8_t *buff, uint8_t len) {
	int ret;
	ret = HAL_I2C_Master_Transmit(&hi2c2, addr, buff, len, 10);
	return ret;
}

int VL6180x_I2CRead(VL6180xDev_t addr, uint8_t *buff, uint8_t len) {
	int ret;
	ret = HAL_I2C_Master_Receive(&hi2c2, addr, buff, len, 10);
	return ret;
}
