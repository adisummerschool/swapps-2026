#include "libfunc.h"
#include <stdio.h>
#include <iio.h>

void func ()
{
	struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.04");
	if(!context) {
		printf("Unable to create context\n");
		return;
	}
	struct iio_device  *device  = iio_context_find_device(context, "ad5592r_s");
	if(!device) {
		printf("Unable to find device\n");
		return;
	}
	struct iio_channel *channel = iio_device_find_channel(device, "voltage0", false);
	if(!channel) {
		printf("Unable to find channel\n");
		return;
	}
	long long raw_value = 0;
	int ret;
	ret = iio_channel_attr_read_longlong(channel, "raw", &raw_value);
	if (ret < 0)
		printf("Error reading raw attribute: %d\n", -ret);
	else
		printf("Raw value: %u\n", raw_value);
}