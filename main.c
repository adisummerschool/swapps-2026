#include <stdio.h>
#include <iio.h>

int main() {
    printf("Hello World!\n");
    printf("%d\n", iio_get_backends_count());

    struct iio_context* ctx = iio_create_context_from_uri("ip:10.76.84.35");
    struct iio_device* dev = iio_context_find_device(ctx, "ad5592r_s");
    struct iio_channel* chn = iio_device_find_channel(dev, "voltage0", false);
    long long raw_value = 0;
    iio_channel_attr_read_longlong(chn, "raw", &raw_value);
    printf("ad5592r_s voltage0 raw value: %lld\n", raw_value);
    return 0;
}