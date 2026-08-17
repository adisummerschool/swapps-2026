#include <stdio.h>
#include <iio.h>

//TASK:
// Read iio.h (see slack) and find a way to read the raw attribute from the voltage0 channel from
// <your device here> by using your board URI and libiio functions. Print the raw value
// that was read to stdout (printf) and you can go home

int main() {
	printf("Hello, World!\n");

	// The number of backends available
	printf("%d\n", iio_get_backends_count());

	// Create a context from the URI of the device
	struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.04");
	if(!context) {
		printf("Unable to create context\n");
		return -1;
	}
	// Find the device by name
	struct iio_device  *device  = iio_context_find_device(context, "ad5592r_s");
	if(!device) {
		printf("Unable to find device\n");
		return -1;
	}
	// Find the channel by name
	struct iio_channel *channel = iio_device_find_channel(device, "voltage0", false);
	if(!channel) {
		printf("Unable to find channel\n");
		return -1;
	}

	long long raw_value;
	int ret;

	// Read the raw attribute from the channel
	ret = iio_channel_attr_read_longlong(channel, "raw", &raw_value);
	if (ret)
		printf("Error reading raw attribute: %d\n", ret);
	else
		printf("Raw value: %u\n", raw_value);
	return 0;
}