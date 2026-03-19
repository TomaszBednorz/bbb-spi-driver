#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/spi/spi.h>
#include <linux/iio/sysfs.h>

#include "imu_lsm6dso_cfg.h"


static int imu_lsm6dso_read_raw(struct iio_dev *iio_dev, struct iio_chan_spec const *ch, int *val, int *val2, long mask);
static int imu_lsm6dso_write_raw(struct iio_dev *iio_dev, struct iio_chan_spec const *chan, int val, int val2, long mask);
static int imu_lsm6dso_hwfifo_set_watermark(struct iio_dev *iio_dev, unsigned int val);
static int imu_lsm6dso_write_raw_get_fmt(struct iio_dev *indio_dev, struct iio_chan_spec const *chan, long mask);
static ssize_t imu_lsm6dso_sysfs_sampling_frequency_avail(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t imu_lsm6dso_sysfs_scale_avail(struct device *dev, struct device_attribute *attr, char *buf);

static int imu_lsm6dso_read(struct imu_lsm6dso_data *data, unsigned int reg, unsigned int *val);
static int imu_lsm6dso_bulk_read(struct imu_lsm6dso_data *data, unsigned int reg, void *val,  unsigned int len);
static int imu_lsm6dso_write(struct imu_lsm6dso_data *data, unsigned int reg, unsigned int val);
static int imu_lsm6dso_update_bit(struct imu_lsm6dso_data *data, unsigned int reg, unsigned int bitmask);
static int imu_lsm6dso_update_bits(struct imu_lsm6dso_data *data, unsigned int reg, unsigned int mask, unsigned int val);

static const struct regmap_config imu_lsm6dso_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static const struct iio_chan_spec imu_lsm6dso_acc_channels[] = {
	IMU_LSM6DSO_CHANNEL_ACC(IMU_LSM6DSO_REG_OUTX_L_ADDR, IIO_MOD_X, 0),
	IMU_LSM6DSO_CHANNEL_ACC(IMU_LSM6DSO_REG_OUTY_L_ADDR, IIO_MOD_Y, 1),
	IMU_LSM6DSO_CHANNEL_ACC(IMU_LSM6DSO_REG_OUTZ_L_ADDR, IIO_MOD_Z, 2),
	IIO_CHAN_SOFT_TIMESTAMP(3),
};

static IIO_DEV_ATTR_SAMP_FREQ_AVAIL(imu_lsm6dso_sysfs_sampling_frequency_avail);
static IIO_DEVICE_ATTR(in_accel_scale_available, 0444,
		       imu_lsm6dso_sysfs_scale_avail, NULL, 0);

static struct attribute *imu_lsm6dso_acc_attributes[] = {
	&iio_dev_attr_sampling_frequency_available.dev_attr.attr,
	&iio_dev_attr_in_accel_scale_available.dev_attr.attr,
	NULL,
};

static const struct attribute_group imu_lsm6dso_acc_attribute_group = {
	.attrs = imu_lsm6dso_acc_attributes,
};

static const struct iio_info imu_lsm6dso_acc_info = {
	.attrs = &imu_lsm6dso_acc_attribute_group,
	.read_raw = imu_lsm6dso_read_raw,
	.write_raw = imu_lsm6dso_write_raw,
	.hwfifo_set_watermark = imu_lsm6dso_hwfifo_set_watermark,
	.write_raw_get_fmt = imu_lsm6dso_write_raw_get_fmt,
};

static int imu_lsm6dso_read_channel(struct imu_lsm6dso_sensor *sensor, unsigned int reg_addr, int *val)
{
	int err;
	__le16 data;

	err = imu_lsm6dso_bulk_read(sensor->data, reg_addr, &data, sizeof(data)); // Maybe some mutexes/ anablinges ensor is required - TODO documentatuion
	*val = (s16)le16_to_cpu(data);

	(void)err; /* Ignoring error for simplicity, should be handled properly in production code */

	return IIO_VAL_INT;
}

static int imu_lsm6dso_read_raw(struct iio_dev *iio_dev, struct iio_chan_spec const *ch, int *val, int *val2, long mask)
{
	struct imu_lsm6dso_sensor *sensor = iio_priv(iio_dev);
	int ret;

	switch (mask) {
		case IIO_CHAN_INFO_RAW:
			ret = iio_device_claim_direct_mode(iio_dev);
			if (ret)
				break;
			ret = imu_lsm6dso_read_channel(sensor, (unsigned int)ch->address, val);
			iio_device_release_direct_mode(iio_dev);
			break;
		case IIO_CHAN_INFO_SAMP_FREQ:
			*val = sensor->odr / 1000;
			*val2 = (sensor->odr % 1000) * 1000;
			ret = IIO_VAL_INT_PLUS_MICRO;
			break;
		case IIO_CHAN_INFO_SCALE:
			*val = 0;
			*val2 = sensor->gain;
			ret = IIO_VAL_INT_PLUS_MICRO;
			break;
		default:
			ret = -EINVAL;
			break;
	}

	return ret;
}

static int imu_lsm6dso_write_raw(struct iio_dev *iio_dev, struct iio_chan_spec const *chan, int val, int val2, long mask)
{
	return 0;
}

static int imu_lsm6dso_hwfifo_set_watermark(struct iio_dev *iio_dev, unsigned int val)
{
	return 0;
}

static int imu_lsm6dso_write_raw_get_fmt(struct iio_dev *indio_dev, struct iio_chan_spec const *chan, long mask)
{
	return 0;
}

static ssize_t imu_lsm6dso_sysfs_sampling_frequency_avail(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct imu_lsm6dso_sensor *sensor = iio_priv(dev_get_drvdata(dev));
	const struct imu_lsm6dso_odr_table *odr_table;
	int i, len = 0;

	odr_table = &sensor->data->settings->odr_tab[sensor->id];

	for (i = 0; i < odr_table->size; i++) 
	{
		len += scnprintf(buf + len, PAGE_SIZE - len, 
				"%d.%03d ", 
				odr_table->odr[i].odr_mhz / 1000, 
				odr_table->odr[i].odr_mhz % 1000);
	}

	len += scnprintf(buf + len, PAGE_SIZE - len, "\n");

	return len;
}

static ssize_t imu_lsm6dso_sysfs_scale_avail(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct imu_lsm6dso_sensor *sensor = iio_priv(dev_get_drvdata(dev));
	const struct imu_lsm6dso_gain_table *gain_table;
	int i, len = 0;

	gain_table = &sensor->data->settings->gain_tab[sensor->id];

	for (i = 0; i < gain_table->size; i++) 
	{
		len += scnprintf(buf + len, PAGE_SIZE - len, 
				"0.%06u ", 
				gain_table->gain[i].gain);
	}

	len += scnprintf(buf + len, PAGE_SIZE - len, "\n");

	return len;
}

static int imu_lsm6dso_read(struct imu_lsm6dso_data *data, unsigned int reg, unsigned int *val)
{
	return regmap_read(data->regmap, reg, val);
}

static int imu_lsm6dso_bulk_read(struct imu_lsm6dso_data *data, unsigned int reg, void *val,  unsigned int len)
{
	return regmap_bulk_read(data->regmap, reg, val, len);
}

static int imu_lsm6dso_write(struct imu_lsm6dso_data *data, unsigned int reg, unsigned int val)
{
	return regmap_write(data->regmap, reg, val);
}

static int imu_lsm6dso_update_bit(struct imu_lsm6dso_data *data, unsigned int reg, unsigned int bitmask)
{
	return regmap_update_bits(data->regmap, reg, bitmask, bitmask);
}

static int imu_lsm6dso_update_bits(struct imu_lsm6dso_data *data, unsigned int reg, unsigned int mask, unsigned int val)
{
	return regmap_update_bits(data->regmap, reg, mask, val);
}

static int imu_lsm6dso_check_whoami(struct imu_lsm6dso_data *data)
{
	int err;
    unsigned int reg_val;

	err = imu_lsm6dso_read(data, IMU_LSM6DSO_REG_WHO_AM_I_ADDR, &reg_val);

	if (err < 0) {
		dev_err(data->dev, "Failed to read whoami register\n");
		return err;
	}

	if (reg_val != IMU_LSM6DSO_REG_WHO_AM_I_VALUE) {
		dev_err(data->dev, "Unexpected WHOAMI value: 0x%02X\n", reg_val);
		return -ENODEV;
	}

	data->settings = &imu_lsm6dso_settings;

	return 0;
}

static int imu_lsm6dso_alloc_iiodev_acc(struct imu_lsm6dso_data *data)
{
	static const unsigned long scan_masks[] = { 0x7, 0x0 };
	struct imu_lsm6dso_sensor *sensor;
	struct iio_dev *iio_dev;

	iio_dev = devm_iio_device_alloc(data->dev, sizeof(*sensor));	


	if(NULL == iio_dev)
	{
		dev_err(data->dev, "Failed to allocate IIO device\n");
		return -ENOMEM;
	}	

	iio_dev->name = "imu-lsm6dso-acc";
	iio_dev->modes = INDIO_DIRECT_MODE;
	iio_dev->available_scan_masks = scan_masks;
	iio_dev->channels = imu_lsm6dso_acc_channels;
	iio_dev->num_channels = ARRAY_SIZE(imu_lsm6dso_acc_channels);  // Try to make it more generic (gyro)
	iio_dev->info = &imu_lsm6dso_acc_info;

	sensor = iio_priv(iio_dev);
	sensor->id = IMU_LSM6DSO_ID_ACC;
	sensor->data = data;
	sensor->odr = data->settings->odr_tab[sensor->id].odr[0].odr_mhz;
	sensor->gain = data->settings->gain_tab[sensor->id].gain[0].gain;

	data->iio_dev_acc = iio_dev;

	return 0;
}

static int imu_lsm6dso_init(struct imu_lsm6dso_data *data)
{
	int err;

	/* IMU SW reset */
	err = imu_lsm6dso_update_bit(data, IMU_LSM6DSO_REG_CTRL3_ADDR, IMU_LSM6DSO_REG_CTRL3_RESET_BITMASK);
	if (err < 0)
		return err;

	msleep(50);

	/* Reboot memory content */
	err = imu_lsm6dso_update_bit(data, IMU_LSM6DSO_REG_CTRL3_ADDR, IMU_LSM6DSO_REG_CTRL3_BOOT_BITMASK);
	if (err < 0)
		return err;

	msleep(50);

	/* Enable block data update */
	err = imu_lsm6dso_update_bit(data, IMU_LSM6DSO_REG_CTRL3_ADDR, IMU_LSM6DSO_REG_CTRL3_BDU_BITMASK);
	if (err < 0)
		return err;

	return 0;
}


static int imu_lsm6dso_probe(struct spi_device *spi)
{
	struct regmap *regmap;
	struct imu_lsm6dso_data *imu_data;
    int err;
    
    pr_info("%s: probe called for SPI device %s (CS=%d)\n", DRIVER_NAME, dev_name(&spi->dev), spi->chip_select);

    /* Initialize the SPI regmap */
	regmap = devm_regmap_init_spi(spi, &imu_lsm6dso_regmap_config);
	if (IS_ERR(regmap)) 
	{
		dev_err(&spi->dev, "Failed to register spi regmap %ld\n", PTR_ERR(regmap));
		return PTR_ERR(regmap);
	}

    /* Allocate driver data structure */
	imu_data = devm_kzalloc(&spi->dev, sizeof(*imu_data), GFP_KERNEL);
	if (NULL == imu_data)
    {
        dev_err(&spi->dev, "Failed to allocate driver data\n");
		return -ENOMEM;
    }
	dev_set_drvdata(&spi->dev, (void *)imu_data);

    imu_data->dev = &spi->dev;
    imu_data->regmap = regmap;

    /* Check the WHOAMI register to verify communication with the device */
	err = imu_lsm6dso_check_whoami(imu_data);
	if (err < 0)
        return err;

	err = imu_lsm6dso_alloc_iiodev_acc(imu_data);
	if(err < 0)	
		return err;

	err = imu_lsm6dso_init(imu_data);
	if (err < 0)
		return err;

	if(NULL != imu_data->iio_dev_acc)
	{
		err = devm_iio_device_register(&spi->dev, imu_data->iio_dev_acc);
		if (err < 0) 
		{
			dev_err(&spi->dev, "Failed to register IIO device: %d\n", err);
			return err;
		}
	}

	pr_info("%s: probe successful\n", DRIVER_NAME);
	return 0;
}

static int imu_lsm6dso_remove(struct spi_device *spi)
{
    pr_info("%s: remove called\n", DRIVER_NAME);
    return 0;
}

/* Device Tree match table */
static const struct of_device_id imu_lsm6dso_dt_ids[] = {
    { .compatible = "custom,imu-lsm6dso", },
	{ } /* Null termination */
};
MODULE_DEVICE_TABLE(of, imu_lsm6dso_dt_ids);

/* Platform driver struct */
static struct spi_driver imu_lsm6dso_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = imu_lsm6dso_dt_ids,
    },
    .probe = imu_lsm6dso_probe,
    .remove = imu_lsm6dso_remove,
};

module_spi_driver(imu_lsm6dso_driver);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomasz Bednorz");
MODULE_DESCRIPTION("IMU TEST DRIVER LSM6DSO");
