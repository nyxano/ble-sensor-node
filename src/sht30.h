#ifndef SHT30_H
#define SHT30_H

#include <stdint.h>

/* Check that the I2C bus is ready and reset the sensor. Returns 0 on success. */
int sht30_init(void);

/* Read one measurement. Values are in hundredths: 2590 = 25.90 C, 5320 = 53.20 %RH.
 * Returns 0 on success, negative errno on failure (-EIO on CRC mismatch). */
int sht30_read(int32_t *temp_c100, int32_t *rh_c100);

#endif