#include "Wire.h"


#if I2C_HARDWARE==0
#include "math.h"


/* return codes from endTransmission() */
#define SUCCESS   0        /* transmission was successful */
#define ENACKADDR 1        /* received nack on transmit of address */
#define ENACKTRNS 2        /* received nack on transmit of data */

#define I2C_WRITE_FLAG 0
#define I2C_READ_FLAG  1

#define ACKFALSE 0
#define ACKTRUE  1
I2CStruct hi2c2={
	.sda_pin = GPIO_PIN_11,
	.scl_pin = GPIO_PIN_10,
	.sda_port=GPIOB,
	.scl_port=GPIOB
};


/* low level conventions:
 * - SDA/SCL idle high (expected high)
 * - always start with i2c_delay rather than end
 */
static void controlSDA(I2CStruct *hi2c){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = hi2c->sda_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(hi2c->sda_port, &GPIO_InitStruct);
}
static void releaseSDA(I2CStruct *hi2c){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = hi2c->sda_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(hi2c->sda_port, &GPIO_InitStruct);
}


uint32_t i2cHalfPeriod = (int)(ceil(1000000.0/I2CFREQUENCY/2));


static void    i2c_start(I2CStruct *hi2c);
static void    i2c_stop(I2CStruct *hi2c);
static int 	i2c_get_ack(I2CStruct *hi2c);
static void    i2c_send_ack(I2CStruct *hi2c);
static void    i2c_send_nack(I2CStruct *hi2c);
static uint8_t   i2c_shift_in(I2CStruct *hi2c);
static void    i2c_shift_out(I2CStruct *hi2c, uint8_t val);

void i2c_start(I2CStruct *hi2c) {
	delay_us(i2cHalfPeriod);
	HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, 1);
	delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 1);
    delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, 0);
    delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 0);
}

void i2c_stop(I2CStruct *hi2c) {
	delay_us(i2cHalfPeriod);
	HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, 0);
	delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 0);
    delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 1);
	delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, 1);
}

int i2c_get_ack(I2CStruct *hi2c) {
	int time = 0;
	releaseSDA(hi2c);
	delay_us(i2cHalfPeriod);
	HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 1);
	delay_us(i2cHalfPeriod);
	HAL_GPIO_ReadPin(hi2c->sda_port, hi2c->sda_pin);
	HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 0);
	controlSDA(hi2c);
	return ACKTRUE;

}

void i2c_send_ack(I2CStruct *hi2c) {
    delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, 0);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 1);
    delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 0);
}


void i2c_send_nack(I2CStruct *hi2c) {
    delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, 1);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 1);
    delay_us(i2cHalfPeriod);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 0);


}

uint8_t i2c_shift_in(I2CStruct *hi2c) {
    uint8_t data = 0;
    int i;
    releaseSDA(hi2c);
    for (i=0;i<8;i++) {
    	delay_us(i2cHalfPeriod);
        HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 1);
        delay_us(i2cHalfPeriod);
        data += HAL_GPIO_ReadPin(hi2c->sda_port, hi2c->sda_pin) << (7-i);
        HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 0);
    }
    controlSDA(hi2c);
    return data;
}

void i2c_shift_out(I2CStruct *hi2c, uint8_t val) {
    int i;
    for (i=0;i<8;i++) {
        delay_us(i2cHalfPeriod);
        HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, !!(val & (1 << (7 - i))));
        delay_us(i2cHalfPeriod);
        HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin,1);
        delay_us(i2cHalfPeriod);
        HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, 0);

    }
}





uint8_t HAL_I2C_Master_Transmit(I2CStruct *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout){
	i2c_start(hi2c);
	i2c_shift_out(hi2c, DevAddress | I2C_WRITE_FLAG);
	if (!i2c_get_ack(hi2c)) return ENACKADDR;

	// shift out the address we're transmitting to
	int i=0;
	while(i<Size) {
		i2c_shift_out(hi2c, pData[i]);
		i++;
		if (!i2c_get_ack(hi2c))
			return ENACKTRNS;
	}
	i2c_stop(hi2c);
	return SUCCESS;
 }

uint8_t HAL_I2C_Master_Receive(I2CStruct *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	int i=0;
	i2c_start(hi2c);

	i2c_shift_out(hi2c, DevAddress | I2C_READ_FLAG);
	if (!i2c_get_ack(hi2c)) return ENACKADDR;

	while(i<Size) {
		pData[i]= i2c_shift_in(hi2c);
		if(i<Size-1){
			i2c_send_ack(hi2c);
			i++;
		}
		else{
			i2c_send_nack(hi2c);
			break;
		}
	}

	i2c_stop(hi2c);
	return SUCCESS;
}


uint8_t HAL_I2C_Mem_Write(I2CStruct *hi2c, uint16_t DevAddress, uint16_t RegAddress, uint16_t addSize,uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	i2c_start(hi2c);

	i2c_shift_out(hi2c, DevAddress | I2C_WRITE_FLAG);
	if(!i2c_get_ack(hi2c)) return ENACKADDR;

	i2c_shift_out(hi2c, RegAddress);
	if(!i2c_get_ack(hi2c)) return ENACKADDR;

	int ret=HAL_I2C_Master_Transmit(hi2c,DevAddress,pData,Size,Timeout);

	return ret;

}

uint8_t HAL_I2C_Mem_Read(I2CStruct *hi2c, uint16_t DevAddress, uint16_t RegAddress,uint16_t addSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	i2c_start(hi2c);

	i2c_shift_out(hi2c,  DevAddress | I2C_WRITE_FLAG);
	if (!i2c_get_ack(hi2c)) return ENACKADDR;

	i2c_shift_out(hi2c, RegAddress);
	if (!i2c_get_ack(hi2c)) return ENACKADDR;

	int ret=HAL_I2C_Master_Receive(hi2c,DevAddress,pData,Size,Timeout);
	return ret;
}


void HAL_I2C_ErrorCallback(I2CStruct *hi2c){
//	uint32_t SDA_PIN = GPIO_PIN_11;
//	uint32_t SCL_PIN = GPIO_PIN_10;
//	GPIO_TypeDef *SDA_PORT=GPIOB;
//	GPIO_TypeDef *SCL_PORT=GPIOB;
//	GPIO_InitTypeDef GPIO_InitStructure;
//	hi2c->Instance->CR1 &= ~I2C_CR1_PE;
//	GPIO_InitStructure.Mode         = GPIO_MODE_OUTPUT_OD;
//	GPIO_InitStructure.Pull         = GPIO_PULLUP;
//	GPIO_InitStructure.Speed        = GPIO_SPEED_FREQ_HIGH;
//	GPIO_InitStructure.Pin          = SCL_PIN;
//	HAL_GPIO_Init(SCL_PORT, &GPIO_InitStructure);
//	HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET);
//	GPIO_InitStructure.Pin          = SDA_PIN;
//	HAL_GPIO_Init(SDA_PORT, &GPIO_InitStructure);
//	HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET);
//	GPIO_InitStructure.Mode         = GPIO_MODE_AF_OD;
//	GPIO_InitStructure.Pin          = SCL_PIN;
//	HAL_GPIO_Init(SCL_PORT, &GPIO_InitStructure);
//	GPIO_InitStructure.Pin          = SDA_PIN;
//	HAL_GPIO_Init(SDA_PORT, &GPIO_InitStructure);
//	hi2c->Instance->CR1 |= I2C_CR1_SWRST;
//	asm("nop");
//	hi2c->Instance->CR1 &= ~I2C_CR1_SWRST;
//	asm("nop");
//	hi2c->Instance->CR1 |= I2C_CR1_PE;
//	HAL_I2C_Init(hi2c);
}

#endif
