#include "libad5592r.h"
#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define CALIB_THRESHOLD	20
#define CALIB_CONSEC_RD	10
#define GRAV_ACCEL	2048
#define SHOCK_THRESH_H	1
#define SHOCK_THRESH_L	0.1

int read_ad5592r_chan(struct iio_channel *ch, long long *val) {
	if(iio_channel_attr_read_longlong(ch, "raw", val)) {
		printf("Unable to read channel. Terminating.\n");
		return -1;
	}
	return 0;
}

void calibrate(const char *ip) {
	int index;
	long long pos_val, neg_val, diff;

	struct iio_context *ctx = iio_create_network_context(ip);
	if(ctx == NULL) {
		printf("The remote at %s was not found. Terminating.\n", ip);
		return;
	}
	printf("The remote has %d devices.\n", iio_context_get_devices_count(ctx));

	struct iio_device *dev = iio_context_get_device(ctx, 0);
	if(dev == NULL) {
		printf("The device was not found. Terminating.\n");
		return;
	}
	printf("The device has %d channels.\n", iio_device_get_channels_count(dev));

	struct iio_channel *Xp = iio_device_get_channel(dev, 0);
	if(Xp == NULL) {
		printf("Positive X channel was not found on the device. Terminating.\n");
		return;
	}
	struct iio_channel *Xn = iio_device_get_channel(dev, 1);
	if(Xn == NULL) {
		printf("Negative X channel was not found on the device. Terminating.\n");
		return;
	}
	struct iio_channel *Yp = iio_device_get_channel(dev, 2);
	if(Yp == NULL) {
		printf("Positive Y channel was not found on the device. Terminating.\n");
		return;
	}
	struct iio_channel *Yn = iio_device_get_channel(dev, 3);
	if(Yn == NULL) {
		printf("Negative Y channel was not found on the device. Terminating.\n");
		return;
	}
	struct iio_channel *Zp = iio_device_get_channel(dev, 4);
	if(Zp == NULL) {
		printf("Positive Z channel was not found on the device. Terminating.\n");
		return;
	}
	struct iio_channel *Zn = iio_device_get_channel(dev, 5);
	if(Zn == NULL) {
		printf("Negative Z channel was not found on the device. Terminating.\n");
		return;
	}

	//calibrate X axis
	printf("Starting to calibrate X axis.\n");
	index = 0;
	while(index < CALIB_CONSEC_RD) {
		read_ad5592r_chan(Xp, &pos_val);
		read_ad5592r_chan(Xn, &neg_val);
		diff = (pos_val > neg_val) ? (pos_val - neg_val) : (neg_val - pos_val);
		if(diff < CALIB_THRESHOLD) {
			if(index == 0) {
				printf("Stand still...\n");
			}
			index++;
		} else {
			index = 0;
			if(pos_val > neg_val) {
				printf("Rotate X clockwise. Deviation is %d.\n", diff);
			} else {
				printf("Rotate X counterclockwise. Deviation is %d.\n", diff);
			}
		}
		usleep(200000);
	}
	printf("X axis calibrated successfully.\n");

	//calibrate Y axis
	printf("Starting to calibrate Y axis.\n");
	index = 0;
	while(index < CALIB_CONSEC_RD) {
		read_ad5592r_chan(Yp, &pos_val);
		read_ad5592r_chan(Yn, &neg_val);
		diff = (pos_val > neg_val) ? (pos_val - neg_val) : (neg_val - pos_val);
		if(diff < CALIB_THRESHOLD) {
			if(index == 0) {
				printf("Stand still...\n");
			}
			index++;
		} else {
			index = 0;
			if(pos_val > neg_val) {
				printf("Rotate Y clockwise. Deviation is %d.\n", diff);
			} else {
				printf("Rotate Y counterclockwise. Deviation is %d.\n", diff);
			}
		}
		usleep(200000);
	}
	printf("Y axis calibrated successfully.\n");

	//calibrate Z axis
	printf("Starting to calibrate Z axis.\n");
	index = 0;
	while(index < CALIB_CONSEC_RD) {
		read_ad5592r_chan(Zp, &pos_val);
		read_ad5592r_chan(Zn, &neg_val);
		diff = (pos_val > 2048) ? (pos_val - 2048) : (2048 - pos_val);
		if(neg_val == 0 && diff < CALIB_THRESHOLD) {
			if(index == 0) {
				printf("Stand still...\n");
			}
			index++;
		} else {
			index = 0;
			if(neg_val != 0) {
				printf("The negative value of the Z axis is nonzero.\n");
			} else {
				if(pos_val > 2048) {
					printf("Rotate Z clockwise. Deviation is %d.\n", diff);
				} else {
					printf("Rotate Z counterclockwise. Deviation is %d.\n", diff);
				}
			}
		}
		usleep(200000);
	}
	printf("Z axis calibrated successfully.\n");
	printf("Calibration ended successfully.\n");
}

int read_ad5592r(const char *ip) {
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

float absf(float x) {
	return (x > 0) ? x : -x;
}

void detect_ad5592r_shock(const char *ip) {
	struct iio_context *ctx = iio_create_network_context(ip);
	if(ctx == NULL) {
		printf("The remote at %s was not found. Terminating.\n", ip);
		return;
	}
	printf("The remote has %d devices.\n", iio_context_get_devices_count(ctx));

	struct iio_device *dev = iio_context_get_device(ctx, 0);
	if(dev == NULL) {
		printf("The device was not found. Terminating.\n");
		return;
	}
	printf("The device has %d channels.\n", iio_device_get_channels_count(dev));

	for(int i = 0; i < 6; i++) {
		struct iio_channel *ch = iio_device_get_channel(dev, i);
		if(ch == NULL) {
			printf("Failed to get channel %d. Terminating.\n", i);
			return;
		}
		iio_channel_enable(ch);
	}

	int samples = 100;
	struct iio_buffer *buf = iio_device_create_buffer(dev, samples, false);
	if(buf == NULL) {
		printf("The buffer could not be created. Terminating.\n");
		return;
	}

	bool prev_shock;

	while(true) {
		int ret = iio_buffer_refill(buf);
		if(ret < 0) {
			printf("Failed to refill buffer. Terminating.\n");
			return;
		}

		void *start = iio_buffer_start(buf);
		void *end = iio_buffer_end(buf);
		void *crtptr = start;
		uint16_t *val;
		uint16_t Xpval, Xnval, Ypval, Ynval, Zpval, Znval;
		float mag_vect;
		ptrdiff_t pdif = iio_buffer_step(buf);
		prev_shock = false;

		while(crtptr < end) {
			val = (uint16_t *)crtptr;
			Xpval = *(val);
			Xnval = *(val + 1);
			Ypval = *(val + 2);
			Ynval = *(val + 3);
			Zpval = *(val + 4);
			Znval = *(val + 5);
			
			mag_vect = sqrt((Xpval - Xnval) * (Xpval - Xnval) + 
					(Ypval - Ynval) * (Ypval - Ynval) +
					(Zpval - Znval) * (Zpval - Znval))
					/ GRAV_ACCEL;
			
			if(absf(mag_vect - 1) > SHOCK_THRESH_H && (!prev_shock)) {
				printf("Shock of magnitude %.2f detected.\n", mag_vect - 1);
				prev_shock = true;
			}
			if(absf(mag_vect - 1) < SHOCK_THRESH_L && prev_shock) {
				prev_shock = false;
			}
			crtptr += pdif;
		}
	}

	iio_buffer_destroy(buf);
}