#include "libfunc.h"
#include <stdio.h>
#include <iio.h>

void func()
{
        struct iio_context *ctx = iio_create_context_from_uri("ip:10.76.84.14");
        if(!ctx) {
                printf("failed to get ctx\n");
                return;
        }

        struct iio_device *dev = iio_context_find_device(ctx, "iio_ad5592r_s");
                if(!dev) {
                printf("failed to get dev\n");
                return;
        }

        const char *channels[6] = {
                "voltage0",
                "voltage1",
                "voltage2",
                "voltage3",
                "voltage4",
                "voltage5",
        };
        
        printf("\n");
        long long raw = 0;

        for (int i = 0; i <=5; i++ ) {
                struct iio_channel *chn = iio_device_find_channel(dev, channels[i], false);
                if(!chn) {
                printf("failed to get chn\n");
                return;
                }

                int ret = iio_channel_attr_read_longlong(chn, "raw", &raw);
                if(ret < 0) {
                printf("failed to read ret %d\n", -ret);
                return;
                }

                printf("ch[%d]: %lld | ", i, raw);
        }

        printf("\n");
}

