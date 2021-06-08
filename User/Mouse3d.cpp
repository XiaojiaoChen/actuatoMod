/*
 * Mouse3d.cpp
 *
 *  Created on: 2021年5月11日
 *      Author: Xiaojiao Chen
 *       Email: yii402072495@gmail.com
 */

#include "Mouse3d.h"

Mouse3d::Mouse3d() {
	revDataHeader = 0x96;
	revDataTail = 0x8D;

}



void Mouse3d::setZeroPosition() {
	cmd = CMD_REQUEST_DATA;
	uartTransInterface(&cmd, 1);
}

void Mouse3d::setAutoDataOn() {
	cmd = CMD_AUTO_DATA_ON;
	uartTransInterface(&cmd, 1);
}

void Mouse3d::setAutoDataOff() {
	cmd = CMD_AUTO_DATA_OFF;
	uartTransInterface(&cmd, 1);
}

void Mouse3d::updatePosition() {
	cmd = CMD_REQUEST_DATA;
	uartTransInterface(&cmd, 1);
	uartReceiveInterface(ReceiveData, REV_DATA_SIZE);
	if (ReceiveData[0] == revDataHeader
			&& ReceiveData[REV_DATA_SIZE - 1] == revDataTail) {
		if (checksumOK()) {
			X = (int16_t)(((uint16_t) ReceiveData[1]) * 128 + ReceiveData[2])
					- 8192;
			Y = (int16_t)(((uint16_t) ReceiveData[3]) * 128 + ReceiveData[4])
					- 8192;
			Z = (int16_t)(((uint16_t) ReceiveData[5]) * 128 + ReceiveData[6])
					- 8192;
			A = (int16_t)(((uint16_t) ReceiveData[7]) * 128 + ReceiveData[8])
					- 8192;
			B = (int16_t)(((uint16_t) ReceiveData[9]) * 128 + ReceiveData[10])
					- 8192;
			C = (int16_t)(((uint16_t) ReceiveData[11]) * 128 + ReceiveData[12])
					- 8192;
		}
	}
}
int Mouse3d::checksumOK() {
	int ret = 0;
	uint16_t checksum = (((uint16_t) ReceiveData[13]) * 128 + ReceiveData[14]);
	uint32_t sum = 0;
	uint16_t sumRes = 0;
	for (int i = 0; i < 13; i++) {
		sum += ReceiveData[i];
	}
	sum &= 0x3FFF;
	if ((uint16_t) sum == checksum)
		ret = 1;
	return ret;
}


void Mouse3d::uartTransInterface(uint8_t *buf, int num) {
	//Baudrate 38400

}

void Mouse3d::uartReceiveInterface(uint8_t *buf, int num) {
	//Baudrate 38400
}
