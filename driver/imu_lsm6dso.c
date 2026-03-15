#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/spi/spi.h>

#include "imu_lsm6dso_cfg.h"

static const struct regmap_config imu_lsm6dsx_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
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

static int imu_lsm6dso_alloc_iiodev(struct imu_lsm6dso_data *data)
{
	struct imu_lsm6dso_sensor *sensor;

	data->iio_dev = devm_iio_device_alloc(data->dev, sizeof(*sensor));	

	if(NULL == data->iio_dev)
	{
		dev_err(data->dev, "Failed to allocate IIO device\n");
		return -ENOMEM;
	}	


	data->iio_dev->name = "imu-lsm6dso"; // 2 - accel and gyrp

	// Configura accel and guro channels here (not implemented in this snippet)
	// Add a configuration for Accel and gyro

	iio_dev->modes = INDIO_DIRECT_MODE;
	iio_dev->available_scan_masks = st_lsm6dsx_available_scan_masks;
	iio_dev->channels = hw->settings->channels[id].chan;
	iio_dev->num_channels = hw->settings->channels[id].len;

	sensor = iio_priv(iio_dev);
	sensor->id = id;
	sensor->hw = hw;
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
	iio_dev->name = sensor->name;

	return iio_dev;

	return 0;
}

static int imu_lsm6dso_probe(struct spi_device *spi)
{
	struct regmap *regmap;
	struct imu_lsm6dso_data *imu_data;
    int err;
    
    pr_info("%s: probe called for SPI device %s (CS=%d)\n", DRIVER_NAME, dev_name(&spi->dev), spi->chip_select);

    /* Initialize the SPI regmap */
	regmap = devm_regmap_init_spi(spi, &imu_lsm6dsx_regmap_config);
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
