#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <errno.h>
#include <stdlib.h>
static const struct i2c_dt_spec sht = I2C_DT_SPEC_GET(DT_NODELABEL(sht30));

static uint8_t crc8(const uint8_t *data, size_t len);
static int sht30_read_raw(uint8_t buf[6]);
static int sht30_read(int32_t *temp_c100, int32_t *rh_c100);

int main(void)
{
    if (!device_is_ready(sht.bus)) {
        printk("I2C bus not ready\n");
        return 0;
    }

    uint8_t reset_cmd[2] = { 0x30, 0xA2 };

    int ret = i2c_write_dt(&sht, reset_cmd, sizeof(reset_cmd));
    printk("soft reset ret = %d\n", ret);
    k_msleep(2);

    uint8_t test[2] = { 0xBE, 0xEF };
    printk("crc test = 0x%02X (expected 0x92)\n", crc8(test, 2));

    while (1) {
        int32_t t, rh;

        ret = sht30_read(&t, &rh);
        if (ret == 0) {
            printk("T = %s%d.%02d C, RH = %d.%02d %%\n",
                   t < 0 ? "-" : "", abs(t) / 100, abs(t) % 100,
                   rh / 100, rh % 100);
        } else {
            printk("read failed: %d\n", ret);
        }

        k_sleep(K_SECONDS(1));
    }

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

static int sht30_read_raw(uint8_t buf[6])
{
    uint8_t meas_cmd[2] = { 0x24, 0x00 };   /* single shot, high repeatability, stretching disabled */

    int ret = i2c_write_dt(&sht, meas_cmd, sizeof(meas_cmd));
    if (ret < 0) {
        return ret;
    }

    k_msleep(20);                      /* czas pomiaru z datasheetu */

    return i2c_read_dt(&sht, buf, 6);
}

static int sht30_read(int32_t *temp_c100, int32_t *rh_c100)
{
    uint8_t buf[6];

    int ret = sht30_read_raw(buf);
    if (ret < 0) {
        return ret;
    }

    if (crc8(&buf[0], 2) != buf[2] || crc8(&buf[3], 2) != buf[5]) {
        return -EIO;                        
    }

    uint16_t raw_t  = ((uint16_t)buf[0] << 8) | buf[1];
    uint16_t raw_rh = ((uint16_t)buf[3] << 8) | buf[4];

    *temp_c100 = -4500 + (int32_t)((17500 * (int64_t)raw_t) / 65535);
    *rh_c100   = (int32_t)((10000 * (int64_t)raw_rh) / 65535);

    return 0;
}