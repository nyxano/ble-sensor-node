#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

static const struct i2c_dt_spec sht = I2C_DT_SPEC_GET(DT_NODELABEL(sht30));

int main(void)
{
    if (!device_is_ready(sht.bus)) {
        printk("I2C bus not ready\n");
        return 0;
    }

    uint8_t cmd[2] = { 0x30, 0xA2 };   /* soft reset z datasheetu SHT3x */

    int ret = i2c_write_dt(&sht, cmd, sizeof(cmd));
    printk("soft reset ret = %d\n", ret);

    return 0;
}