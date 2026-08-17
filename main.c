#include <stdio.h>
#include <iio.h>

int main(const int argc, const char *argv[]) {
	const char *ip;
	const char my_ip[] = "10.76.84.22";
	if(argc > 1) {
		ip = argv[1];
	} else {
		ip = my_ip;
	}
	struct iio_context *ctx = iio_create_network_context(ip);
	if(ctx == NULL) {
		printf("The remote at %s was not found. Terminating.\n", ip);
		return -1;
	}
	printf("The remote has %d devices.\n", iio_context_get_devices_count(ctx));

	struct iio_device *dev = iio_context_get_device(ctx, 0);
	if(dev == NULL) {
		printf("The device was not found. Terminating.\n");
		return -1;
	}
	printf("The device has %d channels.\n", iio_device_get_channels_count(dev));

	struct iio_channel *ch0 = iio_device_get_channel(dev, 0);
	struct iio_channel *ch1 = iio_device_get_channel(dev, 1);
	struct iio_channel *ch2 = iio_device_get_channel(dev, 2);
	struct iio_channel *ch3 = iio_device_get_channel(dev, 3);
	struct iio_channel *ch4 = iio_device_get_channel(dev, 4);
	struct iio_channel *ch5 = iio_device_get_channel(dev, 5);

	long long val0;
	long long val1;
	long long val2;
	long long val3;
	long long val4;
	long long val5;
	if(iio_channel_attr_read_longlong(ch0, "raw", &val0)) {
		printf("Unable to read channel 0. Terminating.\n");
		return -1;
	}
	if(iio_channel_attr_read_longlong(ch1, "raw", &val1)) {
		printf("Unable to read channel 1. Terminating.\n");
		return -1;
	}
	if(iio_channel_attr_read_longlong(ch2, "raw", &val2)) {
		printf("Unable to read channel 2. Terminating.\n");
		return -1;
	}
	if(iio_channel_attr_read_longlong(ch3, "raw", &val3)) {
		printf("Unable to read channel 3. Terminating.\n");
		return -1;
	}
	if(iio_channel_attr_read_longlong(ch4, "raw", &val4)) {
		printf("Unable to read channel 4. Terminating.\n");
		return -1;
	}
	if(iio_channel_attr_read_longlong(ch5, "raw", &val5)) {
		printf("Unable to read channel 5. Terminating.\n");
		return -1;
	}

	printf("ch0: %d | ch1: %d | ch2: %d | ch3: %d | ch4: %d | ch5: %d\n", val0, val1, val2, val3, val4, val5);

	return 0;
}
