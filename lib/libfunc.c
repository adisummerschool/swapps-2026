#include "libfunc.h"
#include <stdio.h>
#include <iio.h>

void func()
{
    struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.24");
    if(!context) {
        printf("failed to get context");
        return;
    }

    struct iio_device *device = iio_context_find_device(context, "ad5592r_s");
    if(!device) {
        printf("failed to get device");
        return;
    }

    struct iio_channel *channel_0 = iio_device_get_channel(device, 0);
    if(!channel_0) {
        printf("failed to get channel");
        return;
    }

    const char *attr = iio_channel_get_attr(channel_0, 0);
    if(!attr) {
        printf("failed to get attr");
        return;
    }

    long long value = 0;
    int ret = iio_channel_attr_read_longlong(channel_0, attr, &value);
    if(ret < 0) {
        printf("failed to read attr %d", -ret);
        return;
    }

    printf("%lld\n", value);
}