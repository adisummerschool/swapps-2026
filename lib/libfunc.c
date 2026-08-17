#include "libfunc.h"
#include "stdio.h"
#include <iio.h>

void func(){
        struct iio_context *st = iio_create_network_context("10.76.84.210");
        if(!st){
                printf("faild to get context");
                return;
        }
        printf("%d\n", iio_context_get_devices_count(st));

        for(int i = 0; i <= 5; i++){
                struct iio_device *devices = iio_context_get_device(st, 0);
                if(!devices){
                        printf("failed to get device");
                        return;
                }
                struct iio_channel *chan = iio_device_get_channel(devices, i);
                  if(!devices){
                        printf("failed to get channel");
                        return;
                }
                long long data = 0;
                int ret = iio_channel_attr_read_longlong(chan, "raw", &data);
                if(ret < 0){
                        printf("failed to get data %d\n", -ret);
                        return;
                }
                printf("%ld ", data);
        }
}
