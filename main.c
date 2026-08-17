#include<stdio.h>
#include<iio.h>

int main(){

        struct iio_device *indio_dev;
        struct iio_context *iio_cont;
        struct iio_channel *chan0;
        long long val;
        int ret = 0;

        printf("Hello world\n");
        printf("%d\n",iio_get_backends_count());

        iio_cont = iio_create_context_from_uri("ip:10.76.84.21");
        indio_dev = iio_context_find_device(iio_cont, "iio_ad5592r_s");
        chan0 = iio_device_get_channel(indio_dev, 0);
        const char *attr = iio_channel_get_attr(chan0, 0);
        ret = iio_channel_attr_read_longlong(chan0, attr, &val);
        if(ret)
        {
                printf("Getting attribute value failed!");
        }

        printf("The voltage0 value is:%d",val);
        return 0;
}