#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

static const struct i2c_dt_spec sht = I2C_DT_SPEC_GET(DT_NODELABEL(sht30));

static uint8_t crc8(const uint8_t *data, size_t len);

int main(void)
{
    if (!device_is_ready(sht.bus)) {
        printk("I2C bus not ready\n");
        return 0;
    }

    uint8_t cmd[2] = { 0x30, 0xA2 };   /* soft reset z datasheetu SHT3x */

    int ret = i2c_write_dt(&sht, cmd, sizeof(cmd));
    printk("soft reset ret = %d\n", ret);

    uint8_t test[2] = { 0xBE, 0xEF };
    printk("crc test = 0x%02X (expected 0x92)\n", crc8(test, 2));
    return 0;
}

static uint8_t crc8(const uint8_t *data, size_t len){


   uint8_t crc = 0xFF;                      /* wartość początkowa z Table 20 */

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];                         /* XOR z bieżącym bajtem */

        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (uint8_t)((crc << 1) ^ 0x31);   /* wielomian z Table 20 */
            } else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }

    return crc; 



}