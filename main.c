#include <stdio.h>
#include <iio.h>



int main () {
	printf("hello, world!\n");
	printf("%d\n", iio_get_backends_count());
	struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.31");
	struct iio_device *dev = iio_context_find_device(cont, "ad5592r_s");
	struct iio_channel *chan0 = iio_device_get_channel(dev, 0);
	const char *attr = iio_channel_get_attr(chan0, 0);
	long long val = 0;
	iio_channel_attr_read_longlong(chan0, attr, &val);
	printf("%lld\n", val);
	return 0;
}
