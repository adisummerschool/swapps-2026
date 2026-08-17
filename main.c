#include <stdio.h>
#include <iio.h>

int main ()  {
        printf("hello, world!\n");
        printf("%d\n", iio_get_backends_count());

        struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.15");
        struct iio_device *device = iio_context_find_device(context, "ad5592r_s");
        struct iio_channel *channel_0 = iio_device_get_channel(device, 0);
        const char *attr = iio_channel_get_attr(channel_0, 0);
        long long value = 0;
        iio_channel_attr_read_longlong(channel_0, attr, &value);

        printf("%lld\n", value);

        return 0;
}