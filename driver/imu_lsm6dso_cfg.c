#include "imu_lsm6dso_cfg.h"

const struct iio_event_spec imu_lsm6dso_event = {
	.type = IIO_EV_TYPE_THRESH,
	.dir = IIO_EV_DIR_EITHER,
	.mask_separate = BIT(IIO_EV_INFO_VALUE) | BIT(IIO_EV_INFO_ENABLE)
};