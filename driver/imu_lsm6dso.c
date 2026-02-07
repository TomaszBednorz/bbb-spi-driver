#include <linux/kernel.h>
#include<linux/module.h>
#include<linux/of.h>
#include<linux/of_device.h>
#include <linux/spi/spi.h>

#define DRIVER_NAME "imu-lsm6dso"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__



static int imu_lsm6dso_probe(struct spi_device *spi)
{
    pr_info("%s: probe called for SPI device %s (CS=%d)\n",
            DRIVER_NAME, dev_name(&spi->dev), spi->chip_select);
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
