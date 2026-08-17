#include <stdio.h>
#include <iio.h>

int main(){
    printf("hello world!\n");
    printf("%d\n", iio_get_backends_count());

    struct iio_context *my_dev = iio_create_context_from_uri("ip:10.76.84.113");
    struct iio_device *my_dev_name = iio_context_find_device(my_dev, "ad5592r_s");
    struct iio_channel *ch0 = iio_device_get_channel(my_dev_name, 0);
    const char *attr = iio_channel_get_attr(ch0, 0);
    long long val = 0;
    iio_channel_attr_read_longlong(ch0, attr, &val);
    printf("%lld\n", val);
    return 0;
}