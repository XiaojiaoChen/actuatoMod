/*
 * bno055_support.h
 *
 *  Created on: Oct 12, 2020
 *      Author: Xiaojiao Chen
 *       Email: yii402072495@gmail.com
 */

#ifndef BNO055_SUPPORT_H_
#define BNO055_SUPPORT_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include "bno055.h"
#include "messages.h"


 struct bno055_data
 {
	    //Raw
	    struct bno055_accel_t accel_xyz;
	    struct bno055_mag_t mag_xyz;
	    struct bno055_gyro_t gyro_xyz;

	    //Converted Raw
	    struct bno055_accel_double_t d_accel_xyz;
	    struct bno055_mag_double_t d_mag_xyz;
	    struct bno055_gyro_double_t d_gyro_xyz;

	    //Fused Raw
	    struct bno055_euler_t euler_hrp;
	    struct bno055_quaternion_t quaternion_wxyz;//Original quaternion data 2^14LSB.
	    struct bno055_linear_accel_t linear_acce_xyz;
	    struct bno055_gravity_t gravity_xyz;

	    //Converted Fused
	    struct bno055_euler_double_t d_euler_hpr;
	    struct bno055_linear_accel_double_t d_linear_accel_xyz;
	    struct bno055_gravity_double_t d_gravity_xyz;

	    //Customized
	    QUATERNION quaternion_wxyz_16;//Customized quaternion data 2^15 LSB.
	    float quaternion_wxyz_float[4];
 };

 struct bno055_struct{
	 struct bno055_t dev;
	 struct bno055_data data;
 };


 s32 bno055_start(void);
 s32 bno055_read_data(void);
 extern struct bno055_struct bno055;


#ifdef __cplusplus
}
#endif
#endif /* BNO055_SUPPORT_H_ */
