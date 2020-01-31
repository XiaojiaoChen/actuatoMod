/*
 * myUsartFunction.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: 402072495
 */


#include "myUsartFunction.h"
#include "cmsis_os.h"
#include "usart.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>


UART_DEVICE Usart1Device={
		.huart = &huart1
};



/*Redirect printf() by implementing (weak) _write function.
 *Every printf() call would store the output string in TxBuf[], ready for Usart DMA output instead of directly output*/
int _write(int file, char *pSrc, int len)
{
	uint8_t *pDes=Usart1Device.TxBuf[Usart1Device.producerTxBufNum];

	//store the string to next buffer
	memcpy(pDes,pSrc,len);
	*(pDes+len)='\0';
	Usart1Device.countTxBuf[Usart1Device.producerTxBufNum] = len;

	//add one bufferedTxNum, recording how many buffered strings that haven't been sent
	Usart1Device.bufferedTxNum++;

	//Try to send just buffered string if this is the only one
	if(Usart1Device.bufferedTxNum == 1){
		HAL_UART_Transmit_DMA(Usart1Device.huart,pDes,Usart1Device.countTxBuf[Usart1Device.producerTxBufNum]);
	//	Usart1Device.TxStart = micros();
	}
//	else{
//	//TO DO, There is a bug here, when the builtInPWMFrequency is changed, the Usart1Devices would somehow suddenly lost the configurations
//		Usart1Device.bufferedTxNum=Usart1Device.bufferedTxNum;
//	}
	//move producerTxBufNum forward
	Usart1Device.producerTxBufNum++;
	Usart1Device.producerTxBufNum%=UART_TX_BUF_NUM;

//	//Buffered term full, wait for consumer to reduce producerTxBufNum
//	while(Usart1Device.bufferedTxNum > (UART_TX_BUF_NUM-2)){
//		//Danger! May block the main program continuously !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		//This waiting mechanism is to take care of the high frequency output within a short period during the Ethercat Initialization
//		//If the producer is always quicker than consumer, for example a high frequency output ,this function would block the program permanently
//	};
	return len;
}

/*this function would overwrite HAL's weak HAL_UART_TxCpltCallback*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	 /*update information*/
	// Usart1Device.TxEnd = micros();
	 Usart1Device.lastTxTime = Usart1Device.TxEnd - Usart1Device.TxStart;
	 Usart1Device.lastTxCount = Usart1Device.countTxBuf[Usart1Device.consumerTxBufNum];

	/*One consumption done. move consumer forward*/
	Usart1Device.consumerTxBufNum++;
	Usart1Device.consumerTxBufNum%=UART_TX_BUF_NUM;

	/*reduce one bufferedTxNum*/
	 Usart1Device.bufferedTxNum--;

	/*If it is still positive, go on consume next*/
	if(Usart1Device.bufferedTxNum>0){
	//	Usart1Device.TxStart = micros();
		uint8_t *px = &Usart1Device.TxBuf[Usart1Device.consumerTxBufNum][0];
		HAL_UART_Transmit_DMA(Usart1Device.huart,px,Usart1Device.countTxBuf[Usart1Device.consumerTxBufNum]);
	}
}

