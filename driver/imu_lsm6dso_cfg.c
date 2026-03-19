#include "imu_lsm6dso_cfg.h"

/* Accelerometer gain in um/s^2/LSB from full-scale range */
#define ACCEL_GAIN_UMS2(FS_G)	((FS_G) * 9806650U / 32768U)

const struct iio_event_spec imu_lsm6dso_event = {
	.type = IIO_EV_TYPE_THRESH,
	.dir = IIO_EV_DIR_EITHER,
	.mask_separate = BIT(IIO_EV_INFO_VALUE) | BIT(IIO_EV_INFO_ENABLE)
};

const struct imu_lsm6dso_settings imu_lsm6dso_settings = {
	.odr_tab[IMU_LSM6DSO_ID_ACC] = {
		.odr = {
			{ 12500,   0x01 },
			{ 26000,   0x02 },
			{ 52000,   0x03 },
			{ 104000,  0x04 },
			{ 208000,  0x05 },
			{ 416000,  0x06 },
			{ 833000,  0x07 },
			{ 1660000, 0x08 },
			{ 3330000, 0x09 },
			{ 6660000, 0x0A },
		},
		.size = IMU_LSM6DSO_ODR_TABLE_SIZE,
	},
	.odr_tab[IMU_LSM6DSO_ID_GYRO] = {
		.odr = {
			{ 12500,   0x01 },
			{ 26000,   0x02 },
			{ 52000,   0x03 },
			{ 104000,  0x04 },
			{ 208000,  0x05 },
			{ 416000,  0x06 },
			{ 833000,  0x07 },
			{ 1660000, 0x08 },
			{ 3330000, 0x09 },
			{ 6660000, 0x0A },
		},
		.size = IMU_LSM6DSO_ODR_TABLE_SIZE,
	},
	.gain_tab[IMU_LSM6DSO_ID_ACC] = {
		.gain = {
			{ ACCEL_GAIN_UMS2(2),  0x00 },
			{ ACCEL_GAIN_UMS2(16), 0x01 },
			{ ACCEL_GAIN_UMS2(4),  0x02 },
			{ ACCEL_GAIN_UMS2(8),  0x03 },
		},
		.size = IMU_LSM6DSO_GAIN_TABLE_SIZE,
	},
};