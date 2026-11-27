#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <errno.h>
#include "sht30.h"

static const struct i2c_dt_spec sht = I2C_DT_SPEC_GET(DT_NODELABEL(sht30));
int sht30_read(int32_t *temp_c100, int32_t *rh_c100);
static uint8_t crc8(const uint8_t *data, size_t len);
static int sht30_read_raw(uint8_t buf[6]);
int sht30_init(void);

static uint8_t crc8(const uint8_t *data, size_t len)
{
    uint8_t crc = 0xFF;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];

        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (uint8_t)((crc << 1) ^ 0x31);
            } else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }

    return crc;
}

static int sht30_read_raw(uint8_t buf[6])
{
    uint8_t meas_cmd[2] = { 0x24, 0x00 };   /* single shot, high repeatability, no clock stretching */

    int ret = i2c_write_dt(&sht, meas_cmd, sizeof(meas_cmd));
    if (ret < 0) {
        return ret;
    }

    k_msleep(20);                           /* measurement takes up to 15 ms */

    return i2c_read_dt(&sht, buf, 6);
}

int sht30_init(void)
{
    if (!device_is_ready(sht.bus)) {
        return -ENODEV;
    }

    uint8_t reset_cmd[2] = { 0x30, 0xA2 };  /* soft reset */

    int ret = i2c_write_dt(&sht, reset_cmd, sizeof(reset_cmd));
    if (ret < 0) {
        return ret;
    }

    k_msleep(2);                            /* sensor needs up to 1 ms after reset */

    return 0;
}

int sht30_read(int32_t *temp_c100, int32_t *rh_c100)
{
    uint8_t buf[6];

    int ret = sht30_read_raw(buf);
    if (ret < 0) {
        return ret;
    }

    if (crc8(&buf[0], 2) != buf[2] || crc8(&buf[3], 2) != buf[5]) {
        return -EIO;                        /* corrupted frame */
    }

    uint16_t raw_t  = ((uint16_t)buf[0] << 8) | buf[1];
    uint16_t raw_rh = ((uint16_t)buf[3] << 8) | buf[4];

    *temp_c100 = -4500 + (int32_t)((17500 * (int64_t)raw_t) / 65535);
    *rh_c100   = (int32_t)((10000 * (int64_t)raw_rh) / 65535);

    return 0;
}