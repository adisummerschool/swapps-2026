#include "libfunc.h"
#include <stdio.h>
#include <iio.h>

void func()
{
    struct iio_context *my_dev = iio_create_context_from_uri("ip:10.76.84.113");
    if(!my_dev) {
        printf("failed to get context");
        return;
    }

    struct iio_device *my_dev_name = iio_context_find_device(my_dev, "ad5592r_s");
    if(!my_dev_name) {
        printf("failed to get device");
        return;
    }

    struct iio_channel *ch0 = iio_device_get_channel(my_dev_name, 0);
    if(!ch0) {
        printf("failed to get channel");
        return;
    }

    const char *attr = iio_channel_get_attr(ch0, 0);
    if(!attr) {
        printf("failed to get attribute");
        return;
    }

    long long val = 0;
    int ret = iio_channel_attr_read_longlong(ch0, attr, &val);
    if(ret < 0) {
        printf("failed to read attribute %d", -ret);
        return;
    }

    printf("%lld\n", val);
}