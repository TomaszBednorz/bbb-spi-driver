#ifndef IMU_LSM6DSO_CFG_H
#define IMU_LSM6DSO_CFG_H

#include <linux/device.h>

#define DRIVER_NAME "imu-lsm6dso"

struct imu_lsm6dso_data {
    struct device *dev;
    struct regmap *regmap;
};

#endif /* IMU_LSM6DSO_CFG_H */