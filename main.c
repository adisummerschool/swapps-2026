#include <stdio.h>
#include <iio.h>

int main () {
        //printf("hello world\n");
        //printf("%d\n", iio_get_backends_count());

        struct iio_context *ctx = iio_create_context_from_uri("ip:10.76.84.14");

        struct iio_device *dev = iio_context_find_device(ctx, "iio_ad5592r_s");

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
                int ret = iio_channel_attr_read_longlong(chn, "raw", &raw);
                printf("ch[%d]: %lld | ", i, raw);
        }

        printf("\n");


        return 0;
}