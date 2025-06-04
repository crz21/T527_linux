/*
 * bmi160_wrapper.h
 *
 *  Created on: Mar 30, 2022
 *      Author: Ibrahim Ozdemir
 *      GitHub: ibrahimcahit
 */

#ifndef __BMI160_USER_H__
#define __BMI160_USER_H__

#include "bmi160.h"
#include "bmi160_defs.h"

#define TRUE 1
#define FALSE 0
#define PRINTIF_DEBUG 0
// #define BMI160_PERIPHERAL BMI160_SPI_INTF
#define BMI160_PERIPHERAL BMI160_I2C_INTF



typedef struct {
    float BMI160_Ax_f32, BMI160_Ay_f32, BMI160_Az_f32;
    float BMI160_Gx_f32, BMI160_Gy_f32, BMI160_Gz_f32;

    int8_t INIT_OK_i8;
} BMI160_t;

int8_t bmi160ReadAccelGyro(BMI160_t *DataStruct);

#endif /* BMI160_WRAPPER_H_ */
