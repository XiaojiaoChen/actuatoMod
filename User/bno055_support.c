#include "bno055.h"
#include "bno055_support.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"

struct bno055_struct bno055;


s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BNO055_delay_msek(u32 msek);
static s32 bno055_convert_quaternion(struct bno055_data *data);


s32 bno055_start(void)
{
    s32 comres = BNO055_SUCCESS;

	bno055.dev.bus_write = BNO055_I2C_bus_write;
	bno055.dev.bus_read = BNO055_I2C_bus_read;
	bno055.dev.delay_msec = BNO055_delay_msek;
	bno055.dev.dev_addr = BNO055_I2C_ADDR2;

    comres = bno055_init(&(bno055.dev));
    comres += bno055_set_power_mode(BNO055_POWER_MODE_NORMAL);
    comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
  //  comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_AMG);
    return comres;
}


s32 bno055_read_data(void)
{
    s32 comres = BNO055_SUCCESS;

    /*Reading Fused data needs to be in operation mode NDOF*/
  //  comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
    comres += bno055_read_quaternion_wxyz(&(bno055.data.quaternion_wxyz));
    bno055_convert_quaternion(&(bno055.data));
    comres += bno055_read_euler_hrp(&(bno055.data.euler_hrp));
    comres += bno055_read_linear_accel_xyz(&(bno055.data.linear_acce_xyz));
    comres += bno055_convert_double_euler_hpr_rad(&(bno055.data.d_euler_hpr));
    comres += bno055_convert_double_linear_accel_xyz_msq(&(bno055.data.d_linear_accel_xyz));

    /*Reading Raw data needs to be in operation mode MAG*/
    // comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_AMG);
//    comres += bno055_read_accel_xyz(&(bno055.data.accel_xyz));
//    comres += bno055_read_mag_xyz(&(bno055.data.mag_xyz));
//    comres += bno055_read_gyro_xyz(&(bno055.data.gyro_xyz));

    return comres;
}

s32 bno055_convert_quaternion(struct bno055_data *data)
{
	data->quaternion_wxyz_16.imuData[0]=(int16_t)(data->quaternion_wxyz.w*2);
	data->quaternion_wxyz_16.imuData[1]=(int16_t)(data->quaternion_wxyz.x*2);
	data->quaternion_wxyz_16.imuData[2]=(int16_t)(data->quaternion_wxyz.y*2);
	data->quaternion_wxyz_16.imuData[3]=(int16_t)(data->quaternion_wxyz.z*2);

	const double sc=(1.0/(1<<14));
	data->quaternion_wxyz_float[0]=data->quaternion_wxyz.w*sc;
	data->quaternion_wxyz_float[1]=data->quaternion_wxyz.x*sc;
	data->quaternion_wxyz_float[2]=data->quaternion_wxyz.y*sc;
	data->quaternion_wxyz_float[3]=data->quaternion_wxyz.z*sc;
	return 0;
}

s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    s32 BNO055_iERROR = BNO055_INIT_VALUE;
    u8 array[8];
    u8 stringpos = BNO055_INIT_VALUE;

    array[BNO055_INIT_VALUE] = reg_addr;
    for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        array[stringpos + 1] = *(reg_data + stringpos);
    }

    BNO055_iERROR = HAL_I2C_Master_Transmit(&hi2c2, (uint8_t)(dev_addr<<1), array, cnt+1, cnt+1);

    return (s8)BNO055_iERROR;
}

s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    s32 BNO055_iERROR = BNO055_INIT_VALUE;
    u8 array[8] = { BNO055_INIT_VALUE };
    u8 stringpos = BNO055_INIT_VALUE;

    array[BNO055_INIT_VALUE] = reg_addr;

    BNO055_iERROR = HAL_I2C_Mem_Read(&hi2c2, (uint8_t)(dev_addr<<1), reg_addr, I2C_MEMADD_SIZE_8BIT, array,cnt ,cnt);

    for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        *(reg_data + stringpos) = array[stringpos];
    }

    return (s8)BNO055_iERROR;
}

void BNO055_delay_msek(u32 msek)
{
	HAL_Delay(1);
}

