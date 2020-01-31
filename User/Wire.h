#ifndef _WIRE_H_
#define _WIRE_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

#if I2C_HARDWARE==0



#define I2C_MEMADD_SIZE_8BIT 0
#define I2CFREQUENCY 400000

typedef struct I2CStructFLAG{
	uint16_t sda_pin;
	uint16_t scl_pin;
	GPIO_TypeDef *sda_port;
	GPIO_TypeDef *scl_port;
}I2CStruct;

uint8_t HAL_I2C_Master_Transmit(I2CStruct *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t HAL_I2C_Master_Receive(I2CStruct *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t HAL_I2C_Mem_Write(I2CStruct *hi2c, uint16_t DevAddress, uint16_t RegAddress,uint16_t addSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t HAL_I2C_Mem_Read(I2CStruct *hi2c, uint16_t DevAddress, uint16_t RegAddress, uint16_t addSize,uint8_t *pData, uint16_t Size, uint32_t Timeout);
void HAL_I2C_ErrorCallback(I2CStruct *hi2c);
extern I2CStruct hi2c2;
#endif

#ifdef __cplusplus
}
#endif


#endif // _WIRE_H_
