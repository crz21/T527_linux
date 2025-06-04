/*
 * bmi160_wrapper.c
 *
 *  Created on: Mar 30, 2022
 *      Author: Ibrahim Ozdemir
 *      GitHub: ibrahimcahit
 */

#include "bmi160_user.h"

#include "FreeRTOS.h"
#include "stm32u0xx_hal.h"
#include "task.h"
#define NOP (0xFF)
struct bmi160_dev sensor;
struct bmi160_sensor_data accel;
struct bmi160_sensor_data gyro;
struct bmi160_foc_conf foc_conf;
struct bmi160_offsets offsets;
struct bmi160_int_settg int_config;
BMI160_t imu_t;
float aX_f32, aY_f32, aZ_f32;
float gX_f32, gY_f32, gZ_f32;

float accel_roll_f32, accel_pitch_f32;
float gyro_roll_f32, gyro_pitch_f32;

float acc_total_vector_f32 = 0;

float yaw_f32, pitch_f32, roll_f32;
uint64_t timer_u64 = 0;
uint64_t lastTime_u64 = 0;

uint8_t set_gyro_angles_u8 = 0;

uint32_t loopHz_u64, loopTime_u64;
// Set initial input parameters
enum BMI160_Ascale { AFS_RAW = 0, AFS_2G, AFS_4G, AFS_8G, AFS_16G };

enum BMI160_Gscale { GFS_RAW = 0, GFS_125DPS, GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS };

// Set sensor range. This is for harware sensitivity
uint8_t BMI160_Asens = AFS_2G;
uint8_t BMI160_Gsens = GFS_1000DPS;

// Specify correct scale for readings.
// This is for post-reading calculations
// If you want raw readings, just select _RAW
uint8_t BMI160_Ascale = AFS_2G;
uint8_t BMI160_Gscale = GFS_1000DPS;

uint8_t BMI160_Ascale_bit, BMI160_Gscale_bit;

float bmi160_aRes, bmi160_gRes;

static TaskHandle_t m_lbmi160_thread;

uint8_t SPIx_ReadWriteByte(SPI_HandleTypeDef *hspi, uint8_t byte)
{
    uint8_t d_read, d_send = byte;
    if (HAL_SPI_TransmitReceive(hspi, &d_send, &d_read, 1, SPITIMEOUT) != HAL_OK) {
        d_read = 0xFF;
    }
    return d_read;
}

int8_t bmi160_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
#if (BMI160_PERIPHERAL == BMI160_SPI_INTF)
    uint8_t tx_data[50] = {0};
    uint8_t i = 0;

    memset(tx_data, 0xFF, len);
    tx_data[0] = reg_addr;
    CS_ENABLE();
    SPIx_ReadWriteByte(&hspi1, reg_addr);
    for (i = 0; i < len; i++) data[i] = SPIx_ReadWriteByte(&hspi1, NOP);
    CS_DISABLE();
#elif (BMI160_PERIPHERAL == BMI160_I2C_INTF)
    HAL_I2C_Master_Transmit(&hi2c2, BMI160_ADDR, &reg_addr, 1, I2CTIMEOUT);
    HAL_I2C_Master_Receive(&hi2c2, BMI160_ADDR, data, len, I2CTIMEOUT);
#endif

    return 0;
}

int8_t bmi160_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    uint8_t tx_data[50] = {0};
#if (BMI160_PERIPHERAL == BMI160_SPI_INTF)
    uint8_t i = 0;

    memcpy(tx_data, data, len);
    CS_ENABLE();
    SPIx_ReadWriteByte(&hspi1, reg_addr);
    for (i = 0; i < len; i++) SPIx_ReadWriteByte(&hspi1, tx_data[i]);
    CS_DISABLE();
#elif (BMI160_PERIPHERAL == BMI160_I2C_INTF)
    tx_data[0] = reg_addr;
    memcpy(&tx_data[1], data, len);
    HAL_I2C_Master_Transmit(&hi2c2, BMI160_ADDR, tx_data, len + 1, I2CTIMEOUT);
#endif

    return 0;
}

int8_t start_foc()
{
    int8_t rslt = 0;

    /* Enable FOC for accel with target values of z = 1g ; x,y as 0g */
    foc_conf.acc_off_en = BMI160_ENABLE;
    foc_conf.foc_acc_x = BMI160_FOC_ACCEL_0G;
    foc_conf.foc_acc_y = BMI160_FOC_ACCEL_0G;
    foc_conf.foc_acc_z = BMI160_FOC_ACCEL_POSITIVE_G;
    sensor.delay_ms(100);

    /* Enable FOC for gyro */
    foc_conf.foc_gyr_en = BMI160_ENABLE;
    foc_conf.gyro_off_en = BMI160_ENABLE;
    sensor.delay_ms(100);

    rslt = bmi160_start_foc(&foc_conf, &offsets, &sensor);

    return rslt;
}

int8_t bmi160ReadAccelGyro(BMI160_t *DataStruct)
{
    int8_t rslt;

    float ax, ay, az, gx, gy, gz;

    rslt = bmi160_get_sensor_data((BMI160_ACCEL_SEL | BMI160_GYRO_SEL), &accel, &gyro, &sensor);

    ax = (float)accel.x / bmi160_aRes;
    ay = (float)accel.y / bmi160_aRes;
    az = (float)accel.z / bmi160_aRes;

    gx = (float)gyro.x / bmi160_gRes;
    gy = (float)gyro.y / bmi160_gRes;
    gz = (float)gyro.z / bmi160_gRes;

    DataStruct->BMI160_Ax_f32 = ax;
    DataStruct->BMI160_Ay_f32 = ay;
    DataStruct->BMI160_Az_f32 = az;

    DataStruct->BMI160_Gx_f32 = gx;
    DataStruct->BMI160_Gy_f32 = gy;
    DataStruct->BMI160_Gz_f32 = gz;

    return rslt;
}

void set_bmi160_Ares()
{
    switch (BMI160_Asens) {
    case AFS_2G:
        BMI160_Ascale_bit = BMI160_ACCEL_RANGE_2G;
        break;
    case AFS_4G:
        BMI160_Ascale_bit = BMI160_ACCEL_RANGE_4G;
        break;
    case AFS_8G:
        BMI160_Ascale_bit = BMI160_ACCEL_RANGE_8G;
        break;
    case AFS_16G:
        BMI160_Ascale_bit = BMI160_ACCEL_RANGE_16G;
        break;
    }
}

void set_bmi160_Gres()
{
    switch (BMI160_Gsens) {
    case GFS_125DPS:
        BMI160_Gscale_bit = BMI160_GYRO_RANGE_125_DPS;
        break;
    case GFS_250DPS:
        BMI160_Gscale_bit = BMI160_GYRO_RANGE_250_DPS;
        break;
    case GFS_500DPS:
        BMI160_Gscale_bit = BMI160_GYRO_RANGE_500_DPS;
        break;
    case GFS_1000DPS:
        BMI160_Gscale_bit = BMI160_GYRO_RANGE_1000_DPS;
        break;
    case GFS_2000DPS:
        BMI160_Gscale_bit = BMI160_GYRO_RANGE_2000_DPS;
        break;
    }
}

void get_bmi160_Ares()
{
    switch (BMI160_Ascale) {
    case AFS_RAW:
        bmi160_aRes = 1.0f;
        break;
    case AFS_2G:
        bmi160_aRes = 16384.0f;
        break;
    case AFS_4G:
        bmi160_aRes = 8192.0f;
        break;
    case AFS_8G:
        bmi160_aRes = 4096.0f;
        break;
    case AFS_16G:
        bmi160_aRes = 2048.0f;
        break;
    }
}

void get_bmi160_Gres()
{
    switch (BMI160_Gscale) {
    case GFS_RAW:
        bmi160_aRes = 1.0f;
        break;
    case GFS_125DPS:
        bmi160_gRes = 262.4f;
        break;
    case GFS_250DPS:
        bmi160_gRes = 131.2f;
        break;
    case GFS_500DPS:
        bmi160_gRes = 65.6f;
        break;
    case GFS_1000DPS:
        bmi160_gRes = 32.8f;
        break;
    case GFS_2000DPS:
        bmi160_gRes = 16.4f;
        break;
    }
}

void bmi160_thread(void *arg)
{
    int8_t rslt;

    set_bmi160_Ares();
    set_bmi160_Gres();
    get_bmi160_Ares();
    get_bmi160_Gres();

    sensor.id = 0;
    sensor.intf = BMI160_PERIPHERAL;
    sensor.read = bmi160_read;
    sensor.write = bmi160_write;
    sensor.delay_ms = vTaskDelay;
    sensor.read_write_len = 32;

    rslt = bmi160_soft_reset(&sensor);
    sensor.delay_ms(200);
    rslt = bmi160_init(&sensor);

    /********************************************************************/

    uint8_t reg_addr = BMI160_CHIP_ID_ADDR;
    uint8_t chipID = 0;
    uint16_t len = 1;
    rslt = bmi160_get_regs(reg_addr, &chipID, len, &sensor);

    /********************************************************************/

    /* Select the Output data rate, range of accelerometer sensor */
    sensor.accel_cfg.odr = BMI160_ACCEL_ODR_800HZ;  // BMI160_ACCEL_ODR_400HZ
    sensor.delay_ms(100);
    sensor.accel_cfg.range = BMI160_Ascale_bit;
    sensor.delay_ms(100);

    /* Select the power mode of accelerometer sensor */
    sensor.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;
    sensor.delay_ms(100);
    sensor.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;  // BMI160_ACCEL_BW_OSR2_AVG2
    sensor.delay_ms(100);

    /* Select the Output data rate, range of Gyroscope sensor */
    sensor.gyro_cfg.odr = BMI160_GYRO_ODR_800HZ;  // BMI160_GYRO_ODR_400HZ
    sensor.delay_ms(100);
    sensor.gyro_cfg.range = BMI160_Gscale_bit;  // BMI160_GYRO_RANGE_250_DPS
    sensor.delay_ms(100);

    /* Select the power mode of Gyroscope sensor */
    sensor.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;
    sensor.delay_ms(100);
    sensor.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;  // BMI160_GYRO_BW_NORMAL_MODE
    sensor.delay_ms(100);

    /* Set the sensor configuration */
    rslt = bmi160_set_sens_conf(&sensor);

    /********************************************************************/

    rslt = start_foc();

    /********************************************************************/

    /* Select the Interrupt channel/pin */
    int_config.int_channel = BMI160_INT_CHANNEL_1;  // Interrupt channel/pin 1

    /* Select the Interrupt type */
    int_config.int_type = BMI160_ACC_GYRO_DATA_RDY_INT;  // Choosing Any motion interrupt
    /* Select the interrupt channel/pin settings */
    int_config.int_pin_settg.output_en = BMI160_ENABLE;          // Enabling interrupt pins to act as output pin
    int_config.int_pin_settg.output_mode = BMI160_DISABLE;       // Choosing push-pull mode for interrupt pin
    int_config.int_pin_settg.output_type = BMI160_DISABLE;       // Choosing active low output
    int_config.int_pin_settg.edge_ctrl = BMI160_ENABLE;          // Choosing edge triggered output
    int_config.int_pin_settg.input_en = BMI160_DISABLE;          // Disabling interrupt pin to act as input
    int_config.int_pin_settg.latch_dur = BMI160_LATCH_DUR_NONE;  // non-latched output

    /* Set the Any-motion interrupt */
    rslt = bmi160_set_int_config(&int_config, &sensor); /* sensor is an instance of the structure bmi160_dev  */

    imu_t.INIT_OK_i8 = rslt;
    while (rslt == 1);

    for (;;) {
        // Read an process data at 1000 Hz rate
        if (imu_t.INIT_OK_i8 != TRUE) {
            bmi160ReadAccelGyro(&imu_t);

            aX_f32 = imu_t.BMI160_Ax_f32;  // Read RAW and unscaled acceleration values from all 3 axes, unit: (g)
            aY_f32 = imu_t.BMI160_Ay_f32;  //
            aZ_f32 = imu_t.BMI160_Az_f32;  //

            gX_f32 = imu_t.BMI160_Gx_f32 * 0.001f;  // Read scaled gyro values from all 3 axes, unit: (deg/s)
            gY_f32 = imu_t.BMI160_Gy_f32 * 0.001f;  // 0.001 is 1 ms whic is represents 1000 Hz rate
            gZ_f32 = imu_t.BMI160_Gz_f32 * 0.001f;  // Multiply readings with calculation period for integration

            gyro_pitch_f32 += gX_f32;  // integrate gyro readings for pitch and roll calculation
            gyro_roll_f32 += gY_f32;

            gyro_pitch_f32 +=
                gyro_roll_f32 *
                sin(gZ_f32 * 0.01745329f);  // correct X andy Y axis readings with respect to Z axis readings
            gyro_roll_f32 -=
                gyro_pitch_f32 * sin(gZ_f32 * 0.01745329f);  // sin function accepts radians, 0.01745329 = pi / 180

            acc_total_vector_f32 =
                sqrt((aX_f32 * aX_f32) + (aY_f32 * aY_f32) + (aZ_f32 * aZ_f32));  // Calculate total acceleration vector

            accel_pitch_f32 = asin(aY_f32 / acc_total_vector_f32) *
                              57.296f;  // Calculate pitch and roll respect to acceleration readings
            accel_roll_f32 = asin(aX_f32 / acc_total_vector_f32) * -57.296f;

            accel_pitch_f32 -= 0.0f;  // Corrections or acceleration calculations.
            accel_roll_f32 -= 0.0f;   // Leave 0 if accel values are ~0.0 when resting

            // initial pitch and roll readings should be aceel-based
            if (set_gyro_angles_u8) {
                gyro_pitch_f32 = gyro_pitch_f32 * 0.999f +
                                 accel_pitch_f32 * 0.001f;  // to calculate final pitch and roll, we get most of
                gyro_roll_f32 = gyro_roll_f32 * 0.999f +
                                accel_roll_f32 * 0.001f;  // gyro readings and small amount of accel readings
            } else {
                gyro_pitch_f32 = accel_pitch_f32;
                gyro_roll_f32 = accel_roll_f32;
                set_gyro_angles_u8 = 1;
            }

            // integrate calculated pitch and roll with previous values
            pitch_f32 = pitch_f32 * 0.75f + gyro_pitch_f32 * 0.25f;
            roll_f32 = roll_f32 * 0.75f + gyro_roll_f32 * 0.25f;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void bmi160_thread_init(void) { xTaskCreate(bmi160_thread, "bmi160_task", 512, NULL, 5, &m_lbmi160_thread); }
