#include<stdio.h>
#include<iio.h>
// #include<>/
#include<errno.h>
//TASK:
//Read iio.h and find  a way to read the raw attribute from the voltage 0 channel from <your device name here> by using your board URI and libiio functions.
// Print the raw value that was read tp stdout (printf) 
int read_voltage0_raw(const char *uri, long long *value){
    struct iio_context *ctx;
    struct iio_device *dev;
    struct iio_channel *ch;

    int ret;
    ctx = iio_create_context_from_uri(uri);
    if(!ctx){
        return -errno;
    }
    dev = iio_context_find_device(ctx,"ad5592r_s");
    if(!dev){
        return -errno;
    }
    ch = iio_device_find_channel(dev,"voltage0",false);
    if(!ch){
        return -errno;
    }
    printf("%lld\n",* value);
    long long value2 = 0;
    // ret = iio_channel_attr_read_longlong(ch,"raw",value);
    // if(ret < 0){
    //     printf("error\n");
    // }
    iio_context_destroy(ctx);
    return 0;
}
int main(){
    printf("%d\n",iio_get_backends_count());
    long long value = 24;
    if(read_voltage0_raw("serial:/dev/ttyUSB1,115200",&value) < 0){
        printf("Error in reading %d\n",errno);
    }
    printf("Raw value read: %lld",value);
    return 0;
}