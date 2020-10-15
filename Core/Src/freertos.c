/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "myUsartFunction.h"
#include "sensors.h"
#include "math.h"
int32_t sensorTime = 0;
int32_t pressurePa = 0;
int32_t c1, c2;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId sensorTaskHandle;
uint32_t sensorTaskBuffer[ 1024 ];
osStaticThreadDef_t sensorTaskControlBlock;
osThreadId sendTaskHandle;
uint32_t sendTaskBuffer[ 1024 ];
osStaticThreadDef_t sendTaskControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void sensorTaskFunc(void const * argument);
void sendTaskFunc(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of sensorTask */
  osThreadStaticDef(sensorTask, sensorTaskFunc, osPriorityAboveNormal, 0, 1024, sensorTaskBuffer, &sensorTaskControlBlock);
  sensorTaskHandle = osThreadCreate(osThread(sensorTask), NULL);

  /* definition and creation of sendTask */
  osThreadStaticDef(sendTask, sendTaskFunc, osPriorityBelowNormal, 0, 1024, sendTaskBuffer, &sendTaskControlBlock);
  sendTaskHandle = osThreadCreate(osThread(sendTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_sensorTaskFunc */
/**
 * @brief Function implementing the sensorTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_sensorTaskFunc */
void sensorTaskFunc(void const * argument)
{
  /* USER CODE BEGIN sensorTaskFunc */
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t sensorTaskPeriod = pdMS_TO_TICKS(40); //20->100

	/* Infinite loop */
	for (;;) {
		c1 = HAL_GetTick();
		tryReadSensors();
		canSend();
		c2 = HAL_GetTick();

		vTaskDelayUntil(&xLastWakeTime, sensorTaskPeriod);
	}
  /* USER CODE END sensorTaskFunc */
}

/* USER CODE BEGIN Header_sendTaskFunc */
/**
 * @brief  Function implementing the sendTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_sendTaskFunc */
void sendTaskFunc(void const * argument)
{
  /* USER CODE BEGIN sendTaskFunc */
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t sendTaskPeriod = pdMS_TO_TICKS(200); //50->200
	/*	extern int32_t elapTime;
	 extern uint16_t laserDis;
	 extern float filteredrange;
	 extern float testDistance[];
	 extern float firrange;*/
	 uint16_t senDis=0;
	 extern int sensorRet[];
	/* Infinite loop */
	for (;;) {
	//	unpackQuaternion(&(sensorDataRx.quaternionCom), (QUATERNION*) imuGetData);
		unpackQuaternion(&(sensorData.quaternionCom), (QUATERNION*) imuOriData);
#if COMPACT_VERSION_PRESSURE_HPA == 1
		senDis= ((uint16_t)((uint16_t)(sensorData.distance)<<3)
				+(uint16_t)((uint16_t)(sensorData.quaternionCom.distanceBit2)<<2)
				+(uint16_t)((uint16_t)(sensorData.quaternionCom.distanceBit1)<<1)
				+(uint16_t)(sensorData.quaternionCom.distanceBit0));
#else
		senDis=sensorData.distance;
#endif

		if (RangeData.errorStatus)
			printf("Laser err:%s\r\n", VL6180x_RangeGetStatusErrString(RangeData.errorStatus)); // your code display error code
		printf("Time:%lu ms, CanID:%d, Pre: %d HPa, Dis: %d mm, Quat: %1.5f %1.5f %1.5f %1.5f\r\n",
				HAL_GetTick(),
				(uint16_t) (canbus.TxHeader.StdId),
				sensorData.pressure,
				senDis,
				imuOriData[0] * 3.051757e-5,
				imuOriData[1] * 3.051757e-5,
				imuOriData[2] * 3.051757e-5,
				imuOriData[3] * 3.051757e-5);

		//      printf("%d %d %d %d\r\n",
		// 			  (int)((round)(RangeData.range_mm*100)),
		//			  sensorData.distance,
		//			  (int)((round)(filteredrange))*100,
		//			  (int)((round)(testDistance[1]*100))
		// 			  );
		vTaskDelayUntil(&xLastWakeTime, sendTaskPeriod);
	}
  /* USER CODE END sendTaskFunc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
