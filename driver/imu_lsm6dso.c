#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/spi/spi.h>
#include <linux/regmap.h>
#include <linux/iio/iio.h>

#include "imu_lsm6dso_cfg.h"

static const struct regmap_config imu_lsm6dsx_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

// update hardcoded values
// add some information as debug info (init sucessful, etc.)
// maybe new file for communication?

static int imu_lsm6dso_check_whoami(struct imu_lsm6dso_data *data)
{
	int err;
    unsigned int reg_val;

	err = regmap_read(data->regmap, 0x0F, &reg_val);

	if (err < 0) {
		dev_err(data->dev, "Failed to read whoami register\n");
		return err;
	}

	if (reg_val != 0x6C) {
		// log
		return -ENODEV;
	}

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
	if (IS_ERR(regmap)) {
		dev_err(&spi->dev, "Failed to register spi regmap %ld\n", PTR_ERR(regmap));
		return PTR_ERR(regmap);
	}

    /* Allocate driver data structure */
	imu_data = devm_kzalloc(&spi->dev, sizeof(*imu_data), GFP_KERNEL);
	if (!imu_data)
    {
        dev_err(&spi->dev, "Failed to allocate driver data\n");
		return -ENOMEM;
    }
	dev_set_drvdata(&spi->dev, (void *)imu_data);

    // mutexes?

    imu_data->dev = &spi->dev;
    imu_data->regmap = regmap;


    /* Check the WHOAMI register to verify communication with the device */
	if (err < imu_lsm6dso_check_whoami(imu_data))
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
