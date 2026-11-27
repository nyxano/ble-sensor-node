#include <zephyr/kernel.h>
#include <stdlib.h>
#include "sht30.h"

int main(void)
{
    int ret = sht30_init();
    if (ret < 0) {
        printk("sht30 init failed: %d\n", ret);
        return 0;
    }

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