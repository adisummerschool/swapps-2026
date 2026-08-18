#include "libfunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iio.h>
#include <math.h>

/*

Itereaza prin fiecare axa (x, y, z) si asteapta pana cand axa e calibrata din potentiometru
. Puteti adauga un sleep intre citiri
. Daca sta 2 secunde (sau x citiri) calibrat atunci trecem la urmatoarea axa
. Puteti face o functie: int get_ch_raw(<ch_name>)
. O axa contine 2 canale si trebuie comparat valorile de pe ele
. Use a threshold (placa are noise): delta < 10
. No buffers

ex: output
calibrate x axis:
turn clockwise
turn clockwise
turn clockwise
turn clockwise
turn clockwise
turn clockwise
Calibrated

calibrate y axis:
turn clockwise
turn clockwise
turn counterclockwise
turn counterclockwise
Calibrated!!!

calibrate z axis:
turn clockwise
turn clockwise
turn counterclockwise
turn counterclockwise
Calibrated!!!

*/

long long get_ch_raw(struct iio_device *device, const char *ch_name) {
    struct iio_channel *channel = iio_device_find_channel(device, ch_name, false);
    if (!channel) {
        return -1;
    }
    long long val = 0;
    if (iio_channel_attr_read_longlong(channel, "raw", &val) < 0) {
        return -1;
    }
    return val;
}

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

void calibrate_func()
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

	const char axis[] = {'x', 'y', 'z'};
	const char *axis_channel[3][2] = {
		{"voltage0", "voltage1"},
		{"voltage2",  "voltage3"},
		{"voltage4",  "voltage5"}
	};

	int threshold = 35;
	int target_stable_count = 20;
	const int OFFSET_1G = 2048;

	for(int i = 0; i < 3; i++) {
		printf("calibrate %c axis:\n", axis[i]);
		int stable_count = 0;

		long long expected_diff = (i == 2) ? OFFSET_1G : 0;

		while(stable_count < target_stable_count) {
			long long val1 = get_ch_raw(device, axis_channel[i][0]);
			long long val2 = get_ch_raw(device, axis_channel[i][1]);

			long long diff = (val1 - val2) - expected_diff;

			if(labs(diff) <= threshold) {
				stable_count++;
			}
			else {
				stable_count = 0;
				if (diff > 0) {
					printf("turn clockwise\n");
				}
				else {
					printf("turn counterclockwise\n");
				}
			}

			usleep(100000);
		}

		printf("Calibrated!!!\n\n");
	}

}

void buffer()
{
	struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.04");
	if(!context) {
		printf("Failed to create context\n");
		return;
	}

	struct iio_device  *device  = iio_context_find_device(context, "ad5592r_s");
	if(!device) {
		printf("Failed to find device\n");
		return;
	}

	struct iio_channel *channel = iio_device_find_channel(device, "voltage0", false);
	if(!channel) {
		printf("Unable to find channel\n");
		return;
	}

	for(int i = 0; i < 6; i++) {
		struct iio_channel *ch = iio_device_get_channel(device, i);
		if(!ch) {
			printf("Failed to get ch %d", i);
			return;
		}
		iio_channel_enable(ch);
	}

	int samples = 100;
	struct iio_buffer* buf = iio_device_create_buffer(device, samples, false);
	if(!buf) {
		printf("Failed to get buffer\n");
		return;
	}

	int ret = iio_buffer_refill(buf);
	if(ret < 0) {
		printf("Failed to refill buffer %d\n", -ret);
		iio_buffer_destroy(buf);
		iio_context_destroy(context);
		return;
	}

	// Pointer to the start of the buffer
	void *start = iio_buffer_start(buf);
	void* end = iio_buffer_end(buf);
	ptrdiff_t step = iio_buffer_step(buf);

	for(void *i = start; i < end; i += step) {
		uint16_t value;
		iio_channel_convert(channel, &value, i);

		printf("%d\n", value);
	}

	// 2048 = 1G
	// magnitudinea vectorului de soc = sqrt(x^2 + y^2 + z^2)
	// shock threshold = 1.5G

	// use a while (true) to read buffers and detect shocks

	// ex output:
	// detected shock: 3.2G
	// detected shock: 1.6G

	iio_buffer_destroy(buf);
	iio_context_destroy(context);
}

void detect_shock()
{
	struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.04");
	if(!context) {
		printf("Failed to create context\n");
		return;
	}

	struct iio_device  *device  = iio_context_find_device(context, "ad5592r_s");
	if(!device) {
		printf("Failed to find device\n");
		return;
	}

	struct iio_channel *ch[6];
	for(int i = 0; i < 6; i++) {
		ch[i] = iio_device_get_channel(device, i);
		if(!ch[i]) {
			printf("Failed to get ch %d", i);
			return;
		}
		iio_channel_enable(ch[i]);
	}

	int samples = 100;
	struct iio_buffer* buf = iio_device_create_buffer(device, samples, false);
	if(!buf) {
		printf("Failed to get buffer\n");
		return;
	}

	const float VALUE_1G = 2048.0f;
	const float THRESHOLD = 1.5f;
	bool in_shock = false;
	float peak_magnitude = 0.0f;

	while(1) {
		int ret = iio_buffer_refill(buf);
		if(ret < 0) {
			printf("Failed to refill buffer %d\n", -ret);
			iio_buffer_destroy(buf);
			iio_context_destroy(context);
			return;
		}
		void *start = iio_buffer_start(buf);
		void* end = iio_buffer_end(buf);
		ptrdiff_t step = iio_buffer_step(buf);

		for(void *i = start; i < end; i += step) {
			uint16_t x, x_min, y, y_min, z, z_min;
			iio_channel_convert(ch[0], &x_min, i);
			iio_channel_convert(ch[1], &x, i+2);
			iio_channel_convert(ch[2], &y, i+4);
			iio_channel_convert(ch[3], &y_min, i+6);
			iio_channel_convert(ch[4], &z, i+8);
			iio_channel_convert(ch[5], &z_min, i+10);
			// printf("DEBUG: x=%d, x_min=%d, y=%d, y_min=%d, z=%d, z_min=%d\n", x, x_min, y, y_min, z, z_min);

			int x_diff = (int)x - (int)x_min;
			int y_diff = (int)y - (int)y_min;
			int z_diff = (int)z - (int)z_min;

			float shock_magnitude = sqrt((float)x_diff*x_diff + (float)y_diff*y_diff + (float)z_diff*z_diff) / VALUE_1G;
			// printf("DEBUG: shock_magnitude=%.2fG\n", shock_magnitude);

			if(shock_magnitude > THRESHOLD) {
				if(!in_shock) {
					in_shock = true;
					peak_magnitude = shock_magnitude;
				} else {
					if(shock_magnitude > peak_magnitude) {
						peak_magnitude = shock_magnitude;
					}
				}
			} else {
				if(in_shock) {
					printf("Detected shock: %.1fG\n", peak_magnitude);
					in_shock = false;
				}
			}
		}
	}
}