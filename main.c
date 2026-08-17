#include <stdio.h>
#include <iio.h>

// TASK:
// Read iio.h and find a way to read the raw attribute from the voltage0 channel from 
// ad5592r_s by using your board URI and libiio functions. Print the raw value that
// was read to stdout (printf).

int main(){
    
    struct iio_context *ctx;
    struct iio_device *dev;
    struct iio_channel *chn;
    long long raw;
    int ret;

    ctx = iio_create_context_from_uri("ip:10.76.84.137");
    if(!ctx){
        printf("context not found");
        return 0;
    }
    dev = iio_context_find_device(ctx, "ad5592r_s");
    if(!dev){
        printf("device not found");
        return 0;
    }
    chn = iio_device_find_channel(dev, "voltage0", false);
    if(!chn){
        printf("channel not found");
        return 0;
    }
    
    ret = iio_channel_attr_read_longlong(chn, "raw", &raw);

    if(ret < 0){
        printf("%d\n", ret);
        iio_context_destroy(ctx);
        return 0;
    }

    printf("%lld\n", raw);

    iio_context_destroy(ctx);

    return 0;
}