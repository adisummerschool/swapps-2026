#include <stdio.h>
#include <iio.h>
#include "libfunc.h"

void func() {
  struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.31");
	if (!cont) {
    printf("Failed to get ctx\n");
    return;
  }

  struct iio_device *dev = iio_context_find_device(cont, "ad5592r_s");
		if (!dev) {
    printf("Failed to get dev\n");
    return;
  }

  struct iio_channel *chan0 = iio_device_get_channel(dev, 0);
		if (!chan0) {
    printf("Failed to get chan0\n");
    return;
  }

  const char *attr = iio_channel_get_attr(chan0, 0);
  	if (!attr) {
    printf("Failed to get attr\n");
    return;
  }

  long long val = 0;
	int ret = iio_channel_attr_read_longlong(chan0, attr, &val);
		if (ret < 0) {
    printf("Failed to read attr %d\n", -ret);
    return;
  }
  printf("%lld\n", val);
}
