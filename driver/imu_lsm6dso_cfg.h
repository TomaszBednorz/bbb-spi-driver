#ifndef IMU_LSM6DSO_CFG_H
#define IMU_LSM6DSO_CFG_H

#include <linux/device.h>
#include <linux/iio/iio.h>
#include <linux/regmap.h>

#include <stdint.h>

#define DRIVER_NAME "imu-lsm6dso"

#define IMU_LSM6DSO_REG_WHO_AM_I_ADDR  (0x0F)


#define IMU_LSM6DSO_REG_WHO_AM_I_VALUE  (0x6C)


struct imu_lsm6dso_data {
    struct device *dev;
    struct regmap *regmap;
    struct iio_dev *iio_dev;
};

/**
 * struct imu_lsm6dso_sensor - IMU LSM6DS sensor instance
 * @hw: Pointer to instance of struct imu_lsm6dso_data.
 * @gain: Configured sensor sensitivity.
 * @odr: Output data rate of the sensor [Hz].
 */
struct imu_lsm6dso_sensor {
    struct imu_lsm6dso_data *data;
    uint32_t gain;
	uint32_t odr;
};

#endif /* IMU_LSM6DSO_CFG_H */