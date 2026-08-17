#include "libfunc.h"
#include <stdio.h>
#include <iio.h>


void func(){
    struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.15");
    if(!context) {
        printf("Unable to create IIO context\n");
        return;
    }

    struct iio_device *device = iio_context_find_device(context, "ad5592r_s");
    if(!device) {
        printf("Unable to find device\n");
        return;
    }

    struct iio_channel *channel_0 = iio_device_get_channel(device, 0);
    if(!channel_0) {
        printf("Unable to find channel 0\n");
        return;
    }
    
    const char *attr = iio_channel_get_attr(channel_0, 0);
    if(!attr) {
        printf("Unable to find attribute\n");
        return;
    }
    
    long long value = 0;
    int ret = iio_channel_attr_read_longlong(channel_0, attr, &value);
    if(ret < 0) {
        printf("Unable to read attribute: %d\n", -ret);
        return;
    }

    printf("%lld\n", value);
}

