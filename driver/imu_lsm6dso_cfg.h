#ifndef IMU_LSM6DSO_CFG_H
#define IMU_LSM6DSO_CFG_H

#include <linux/device.h>
#include <linux/iio/iio.h>
#include <linux/regmap.h>

#define DRIVER_NAME "imu-lsm6dso"

#define IMU_LSM6DSO_REG_WHO_AM_I_ADDR  (0x0F)
#define IMU_LSM6DSO_REG_CTRL3_ADDR    (0x12)
#define IMU_LSM6DSO_REG_OUTX_L_ADDR    (0x28)
#define IMU_LSM6DSO_REG_OUTY_L_ADDR    (0x2A)
#define IMU_LSM6DSO_REG_OUTZ_L_ADDR    (0x2C)

#define IMU_LSM6DSO_REG_WHO_AM_I_VALUE  (0x6C)

#define IMU_LSM6DSO_REG_CTRL3_RESET_BITMASK  (BIT(0))
#define IMU_LSM6DSO_REG_CTRL3_BDU_BITMASK    (BIT(6))
#define IMU_LSM6DSO_REG_CTRL3_BOOT_BITMASK   (BIT(7))

#define IMU_LSM6DSO_ODR_TABLE_SIZE			(10U)
#define IMU_LSM6DSO_GAIN_TABLE_SIZE			(4U)

#define IMU_LSM6DSO_CHANNEL_ACC(addr, mod, scan_idx)		\
{									\
	.type = IIO_ACCEL,						\
	.address = addr,						\
	.modified = 1,							\
	.channel2 = mod,						\
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),			\
	.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),		\
	.info_mask_shared_by_all = BIT(IIO_CHAN_INFO_SAMP_FREQ),	\
	.scan_index = scan_idx,						\
	.scan_type = {							\
		.sign = 's',						\
		.realbits = 16,						\
		.storagebits = 16,					\
		.endianness = IIO_LE,					\
	},								\
	.event_spec = &imu_lsm6dso_event,				\
	.num_event_specs = 1,						\
}

enum imu_lsm6dso_sensor_id {
	IMU_LSM6DSO_ID_ACC = 0,
	IMU_LSM6DSO_ID_GYRO = 1,
	IMU_LSM6DSO_ID_MAX = 2
};

/**
 * struct imu_lsm6dso_odr - Output data rate entry
 * @odr_mhz: Output data rate in milli hertz (mHz).
 * @reg_val: Value to be written to sensor register to set the ODR.
 */
struct imu_lsm6dso_odr
{
	u32 odr_mhz;
	u8 reg_val;
};

/**
 * struct imu_lsm6dso_gain - Sensor gain entry
 * @gain: Sensor gain in physical unit, um/s^2/LSB for accelerometer and ... for gyroscope.
 * @reg_val: Value to be written to sensor register to set the gain.

*/
struct imu_lsm6dso_gain
{
	u32 gain;
	u8 reg_val;
};

struct imu_lsm6dso_odr_table
{
	struct imu_lsm6dso_odr odr[IMU_LSM6DSO_ODR_TABLE_SIZE];
	u32 size;
};

struct imu_lsm6dso_gain_table
{
	struct imu_lsm6dso_gain gain[IMU_LSM6DSO_GAIN_TABLE_SIZE];
	u32 size;
};

struct imu_lsm6dso_settings {
	struct imu_lsm6dso_odr_table odr_tab[IMU_LSM6DSO_ID_MAX];
	struct imu_lsm6dso_gain_table gain_tab[IMU_LSM6DSO_ID_MAX];
};

struct imu_lsm6dso_data {
    struct device *dev;
    struct regmap *regmap;
    struct iio_dev *iio_dev_acc;
    struct iio_dev *iio_dev_gyro;
	const struct imu_lsm6dso_settings *settings;
};

/**
 * struct imu_lsm6dso_sensor - IMU LSM6DS sensor instance
 * @hw: Pointer to instance of struct imu_lsm6dso_data.
 * @gain: Configured sensor sensitivity.
 * @odr: Output data rate of the sensor [mHz].
 */
struct imu_lsm6dso_sensor {
    enum imu_lsm6dso_sensor_id id;
    struct imu_lsm6dso_data *data;
    u32 gain;
	u32 odr;
};

extern const struct iio_event_spec imu_lsm6dso_event;
extern const struct imu_lsm6dso_settings imu_lsm6dso_settings;

#endif /* IMU_LSM6DSO_CFG_H */