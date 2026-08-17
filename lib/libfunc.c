#include "libfunc.h"
#include <stdio.h>
#include <iio.h>

void func(){
    struct iio_context *ctx;
    struct iio_device *dev;
    struct iio_channel *chn;
    long long raw;
    int ret;

    ctx = iio_create_context_from_uri("ip:10.76.84.137");
    if(!ctx){
        printf("context not found");
        return;
    }
    dev = iio_context_find_device(ctx, "ad5592r_s");
    if(!dev){
        printf("device not found");
        return;
    }
    chn = iio_device_find_channel(dev, "voltage0", false);
    if(!chn){
        printf("channel not found");
        return;
    }
    
    ret = iio_channel_attr_read_longlong(chn, "raw", &raw);

    if(ret < 0){
        printf("%d\n", -ret);
        iio_context_destroy(ctx);
        return;
    }

    printf("%lld\n", raw);

    iio_context_destroy(ctx);
}