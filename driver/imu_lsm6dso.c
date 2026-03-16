#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/spi/spi.h>

#include "imu_lsm6dso_cfg.h"

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


static int imu_lsm6dso_check_whoami(struct imu_lsm6dso_data *data)
{
	int err;
    unsigned int reg_val;

	err = regmap_read(data->regmap, IMU_LSM6DSO_REG_WHO_AM_I_ADDR, &reg_val);

	if (err < 0) {
		dev_err(data->dev, "Failed to read whoami register\n");
		return err;
	}

	if (reg_val != IMU_LSM6DSO_REG_WHO_AM_I_VALUE) {
		dev_err(data->dev, "Unexpected WHOAMI value: 0x%02X\n", reg_val);
		return -ENODEV;
	}

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
	iio_dev->num_channels = ARRAY_SIZE(imu_lsm6dso_acc_channels);

	sensor = iio_priv(iio_dev);
	sensor->id = IMU_LSM6DSO_ID_ACC;
	sensor->data = data;


	// continue - add cofg for this
	sensor->odr = hw->settings->odr_table[id].odr_avl[0].milli_hz;
	sensor->gain = hw->settings->fs_table[id].fs_avl[0].gain;
	sensor->watermark = 1;

	switch (id) {
	case ST_LSM6DSX_ID_ACC:
		iio_dev->info = &st_lsm6dsx_acc_info;
		scnprintf(sensor->name, sizeof(sensor->name), "%s_accel",
			  name);
		break;
	case ST_LSM6DSX_ID_GYRO:
		iio_dev->info = &st_lsm6dsx_gyro_info;
		scnprintf(sensor->name, sizeof(sensor->name), "%s_gyro",
			  name);
		break;
	default:
		return NULL;
	}
	// end of config for this


	data->iio_dev_acc = iio_dev;

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
	if (err < imu_lsm6dso_check_whoami(imu_data))
    {
        return err;
    }
		
	if(err < imu_lsm6dso_alloc_iiodev(imu_data))
	{
		return err;
	}



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
