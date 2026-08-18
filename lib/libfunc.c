#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <unistd.h>
#include <math.h>

void func ()
{
	int threshold = 50;
    struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.9");
	if(!cont) {
		printf("failed to get ctx");
		return;
	}

	struct iio_device *dev = iio_context_find_device(cont, "iio_ad5592r_s");
	if(!dev) {
		printf("failed to get dev");
		return;
	}



	struct iio_channel *channels[6];



	for(int i=0; i<6; i++) {
		struct iio_channel *ch = iio_device_get_channel(dev, i);
		if(!ch) {
			printf("failed to get ch %d", i);
			return;
		}

		channels[i] = ch;
	}

	int calibrated;
	for (int i=0; i<6; i+=2){
		calibrated = 0;
		do {
			const char *x1 = iio_channel_get_attr(channels[i], 0);

			long long val1 = 0;
			int ret = iio_channel_attr_read_longlong(channels[i], x1, &val1);
			if(0>ret) {
				printf("failed to get val1");
				return;
			}
			const char *x2 = iio_channel_get_attr(channels[i+1], 0);

			long long val2 = 0;
			ret = iio_channel_attr_read_longlong(channels[i+1], x2, &val2);
			if(0>ret) {
				printf("failed to get val2");
				return;
			}

			if(i<4) {
				if(abs(val1-val2) < threshold) {
					printf("Calibrated %s axis\n", i<2 ? "X" : "Y");
					calibrated = 1;
				} else {
					printf("spin %s; %d - %d\n", val1 > val2 ? "clockwaise" : "counterclockwaise", val1, val2);
					calibrated = 0;
				}
			} else {
				if(val2 < threshold && (abs(val1 - 2048) < threshold)) {
					printf("Calibrated Z axis\n");
					calibrated = 1;
				} else {
					printf("spin %s; %d - %d\n", val1 > val2 ? "clockwaise" : "counterclockwaise", val1, val2);
					calibrated = 0;
				}
			}

			sleep(1);

		} while(!calibrated);
}
}

void buffer()
{
    struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.9");
	if(!cont) {
		printf("failed to get ctx");
		return;
	}
    
   struct iio_device *dev = iio_context_find_device(cont, "iio_ad5592r_s");
	if(!dev) {
		printf("failed to get dev");
		return;
	}

    for(int i=0; i<6; i++) {
		struct iio_channel *ch = iio_device_get_channel(dev, i);
		if(!ch) {
			printf("failed to get ch %d", i);
			return;
		}

		iio_channel_enable(ch);
	}

    int samples = 100;
    struct iio_buffer* buf = iio_device_create_buffer(dev, samples, false);
    if(!buf){
        printf("failed to get buffer");
        return;
    }

	while(true){
		int ret = iio_buffer_refill(buf);
		if(ret < 0){
			printf("failed to refill buffer %d\n", -ret);
			iio_buffer_destroy(buf);
			iio_context_destroy(cont);
			return;
		}

		void *start = iio_buffer_start(buf);
		void *end = iio_buffer_end(buf);
		ptrdiff_t step = iio_buffer_step(buf);

		for(void* p = start; p <= end; p+=step){
			uint16_t val_x0 = ((uint16_t *)p)[0];
			uint16_t val_x1 = ((uint16_t *)p)[1];
			uint16_t val_x = val_x0 + val_x1;

			uint16_t val_y0 = ((uint16_t *)p)[2];
			uint16_t val_y1 = ((uint16_t *)p)[3];
			uint16_t val_y = val_y0 + val_y1;

			uint16_t val_z0 = ((uint16_t *)p)[4];
			uint16_t val_z1 = ((uint16_t *)p)[5];
			uint16_t val_z = val_z0 + val_z1;

			float vector = sqrt (val_x*val_x + val_y*val_y + val_z*val_z);
			float threshold = (float) vector / 2048.0;

			if( threshold > 1.5 || threshold < 0.5){
				printf("detected shock: %.2f G\n", threshold);
			}
		}
	}

	
	int ret = iio_buffer_refill(buf);

    iio_buffer_destroy(buf);
	iio_context_destroy(cont);
}