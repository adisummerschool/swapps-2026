#include "libfunc.h"
#include <stdio.h>
#include <iio.h>

void func() {
    struct iio_context* cont = iio_create_context_from_uri("ip:10.76.84.35");
    if (!cont) {
        printf("Failed to get context");
        return;
    }
    struct iio_device* dev = iio_context_find_device(cont, "ad5592r_s");
    if (!dev) {
        printf("Failed to get device");
        return;
    }
    struct iio_channel *chan0 = iio_device_get_channel(dev, 0);
    if (!chan0) {
        printf("Failed to get channel");
        return;
    }
    const char *attr = iio_channel_get_attr(chan0, 0);
    if (!attr) {
        printf("Failed to get attribute");
        return;
    }
    long long val = 0;
    int ret = iio_channel_attr_read_longlong(chan0, attr, &val);
    if (ret < 0) {
        printf("Failed to get value from attr %d", -ret);
        return;
    }
    printf("%lld\n", val);
}