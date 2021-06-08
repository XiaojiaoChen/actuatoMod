/*
 * Mouse3d.h
 *
 *  Created on: 2021年5月11日
 *      Author: Xiaojiao Chen
 *       Email: yii402072495@gmail.com
 */

#ifndef MOUSE3D_H_
#define MOUSE3D_H_

#include "messages.h"

#define CMD_REQUEST_DATA 0xAC
#define CMD_SET_ZERO_POSITION 0xAD
#define CMD_AUTO_DATA_ON 0xAE
#define CMD_AUTO_DATA_OFF 0xAF
#define REV_DATA_SIZE 16

class Mouse3d {
public:
	Mouse3d();

	int16_t X;
	int16_t Y;
	int16_t Z;
	int16_t A;
	int16_t B;
	int16_t C;
	float Xf;
	float Yf;
	float Zf;
	float Af;
	float Bf;
	float Cf;

	uint8_t cmd;
	uint8_t ReceiveData[REV_DATA_SIZE];
	uint8_t revDataHeader;
	uint8_t revDataTail;

	void setZeroPosition();
	void setAutoDataOn();
	void setAutoDataOff();

	void updatePosition();
	int checksumOK();


	//Baudrate 38400
	void uartTransInterface(uint8_t *buf, int num);
	void uartReceiveInterface(uint8_t *buf, int num);

};




#endif /* MOUSE3D_H_ */
