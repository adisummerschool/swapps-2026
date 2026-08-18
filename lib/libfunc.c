#include <stdio.h>
#include <stdbool.h>
#include <iio.h>
#include <unistd.h>
#include <math.h>
#include "libfunc.h"

#define THRESHOLD 25
#define SHOCK_THRESHOLD 3072

void calibrate_axis(struct iio_channel *chan_pos, struct iio_channel *chan_neg) {
  if (!chan_pos || !chan_neg) {
      printf("Failed to get channel(s), ending calibration.\n");
      return;
  }
  
  long long int last_dif[3] = {100, 100, 100};

  for (;;) {
    const char *attr0 = iio_channel_get_attr(chan_pos, 0);
    const char *attr1 = iio_channel_get_attr(chan_neg, 0);
    
    if (!attr0 || !attr1) {
      printf("Failed to get channel attribute, ending calibration.\n");
      return;
    }

    long long chan0_val = 0;
    long long chan1_val = 0;

    int ret0 = iio_channel_attr_read_longlong(chan_pos, attr0, &chan0_val);
    int ret1 = iio_channel_attr_read_longlong(chan_neg, attr1, &chan1_val);
    
    if (ret0 < 0) {
      printf("Channel 0 read failed, err %d\n", ret0);
      return;
    }
    if (ret1 < 0) {
      printf("Channel 1 read failed, err %d\n", ret1);
      return;
    }

    printf("pos value: %lld | neg value: %lld\n", chan0_val, chan1_val);
    last_dif[2] = last_dif[1];
    last_dif[1] = last_dif[0];
    last_dif[0] = abs(chan0_val - chan1_val);
    if (last_dif[0] < THRESHOLD && last_dif[1] < THRESHOLD && last_dif[2] < THRESHOLD) {
      printf("Finished calibration.\n");
      return;
    }

    if (chan0_val < chan1_val) {
      printf("Please turn knob counter-clockwise.\n");
    } else {
      printf("Please turn knob clockwise.\n");
    }
 
    sleep(1);
  }
}

void calibrate_z_axis(struct iio_channel *chan_pos, struct iio_channel *chan_neg) {
  if (!chan_pos || !chan_neg) {
      printf("Failed to get channel(s), ending calibration.\n");
      return;
  }
  
  long long int last_dif[3] = {100, 100, 100};

  for (;;) {
    const char *attr0 = iio_channel_get_attr(chan_pos, 0);
    const char *attr1 = iio_channel_get_attr(chan_neg, 0);
    
    if (!attr0 || !attr1) {
      printf("Failed to get channel attribute, ending calibration.\n");
      return;
    }

    long long chan0_val = 0;
    long long chan1_val = 0;

    int ret0 = iio_channel_attr_read_longlong(chan_pos, attr0, &chan0_val);
    int ret1 = iio_channel_attr_read_longlong(chan_neg, attr1, &chan1_val);
    
    if (ret0 < 0) {
      printf("Channel pos read failed, err %d\n", ret0);
      return;
    }
    if (ret1 < 0) {
      printf("Channel neg read failed, err %d\n", ret1);
      return;
    }

    last_dif[2] = last_dif[1];
    last_dif[1] = last_dif[0];
    last_dif[0] = abs(chan0_val - 2048);
    printf("Z pos value: %lld | Z neg value: %lld\n", chan0_val, chan1_val);
    if (last_dif[0] < THRESHOLD && last_dif[1] < THRESHOLD && last_dif[2] < THRESHOLD) {
      printf("Finished calibration.\n");
      return;
    }

    if (chan0_val < chan1_val) {
      printf("Please turn knob counter-clockwise.\n");
    } else {
      printf("Please turn knob clockwise.\n");
    }
 
    sleep(1);
  }
}


void calibrate_ad5592r_s() {
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

  printf("===== Calibrating X axis =====\n");
  struct iio_channel *chan_pos = iio_device_get_channel(dev, 0);
		if (!chan_pos) {
    printf("Failed to get x pos channel\n");
    return;
  }
  struct iio_channel *chan_neg = iio_device_get_channel(dev, 1);
		if (!chan_neg) {
    printf("Failed to get x neg channel\n");
    return;
  }
  calibrate_axis(chan_pos, chan_neg);
  printf("Calibrated X axis. Continue with Y.\n");
  
  printf("===== Calibrating Y axis =====\n");
  chan_pos = iio_device_get_channel(dev, 2);
		if (!chan_pos) {
    printf("Failed to get y pos channel\n");
    return;
  }
  chan_neg = iio_device_get_channel(dev, 3);
		if (!chan_neg) {
    printf("Failed to get y neg channel\n");
    return;
  }
  calibrate_axis(chan_pos, chan_neg);
  printf("Calibrated Y axis. Continue with Z.\n");

  printf("===== Calibrating Z axis =====\n");
  chan_pos = iio_device_get_channel(dev, 4);
		if (!chan_pos) {
    printf("Failed to get z pos channel\n");
    return;
  }
  chan_neg = iio_device_get_channel(dev, 5);
		if (!chan_neg) {
    printf("Failed to get z neg channel\n");
    return;
  }
  calibrate_z_axis(chan_pos, chan_neg);
}

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

void buffer() {
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

  struct iio_channel *chan[6];
  for(int i = 0; i < 6; ++i) {
    chan[i] = iio_device_get_channel(dev, i);
		if (!chan[i]) {
      printf("Failed to get chan %d.\n", i);
      return;
    }
    iio_channel_enable(chan[i]);
  }

  int samples = 100;
  struct iio_buffer *buf = iio_device_create_buffer(dev, samples, false);

  if (!buf) {
    printf("Failed to get buffer.\n");
    return;
  }

  double g_x = 0.0, g_y = 0.0, g_z = 0.0;
  const double ALPHA = 0.9;

  for (;;) {
    int ret = iio_buffer_refill(buf);
    if (ret < 0) {
      printf("buf read failed with err code %d.\n", -ret);
      return;
    }

    void *start = iio_buffer_start(buf);
    void *end = iio_buffer_end(buf);
    ptrdiff_t step = iio_buffer_step(buf);
  
    uint16_t val[6] = {0, 0, 0, 0, 0, 0};

    for(void *ptr = start; ptr < end; ptr += step) {
      iio_channel_convert(chan[0], &val[0], ptr);
      iio_channel_convert(chan[1], &val[1], ptr + 1 * sizeof(uint16_t));
      iio_channel_convert(chan[2], &val[2], ptr + 2 * sizeof(uint16_t));
      iio_channel_convert(chan[3], &val[3], ptr + 3 * sizeof(uint16_t));
      iio_channel_convert(chan[4], &val[4], ptr + 4 * sizeof(uint16_t));
      iio_channel_convert(chan[5], &val[5], ptr + 5 * sizeof(uint16_t));

      int32_t ax = val[0] - val[1];
      int32_t ay = val[2] - val[3];
      int32_t az = val[4] - val[5];

      g_x = ALPHA * g_x + (1.0 - ALPHA) * (double)ax;
      g_y = ALPHA * g_y + (1.0 - ALPHA) * (double)ay;
      g_z = ALPHA * g_z + (1.0 - ALPHA) * (double)az;

      double shock_x = (double)ax - g_x;
      double shock_y = (double)ay - g_y;
      double shock_z = (double)az - g_z;

      uint32_t shock_vector = sqrt(shock_x * shock_x +
                                   shock_y * shock_y +
                                   shock_z * shock_z);

      if (shock_vector > SHOCK_THRESHOLD) {
        printf("DETECTED SHOCK: %.1f G\n", shock_vector / 2048.f);
      }
    }
    
  }
  iio_buffer_destroy(buf);
}
