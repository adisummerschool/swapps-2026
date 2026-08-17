#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

int get_chan_raw(struct iio_context *context, int channel_num, int *value) {
    struct iio_device *device = iio_context_find_device(context, "ad5592r_s");

    if(!device) {
        printf("failed to get device");
        return -1;
    }

    struct iio_channel *channel = iio_device_get_channel(device, channel_num);

    if(!channel) {
        printf("failed to get channel");
        return -1;
    }

    const char *attr = iio_channel_get_attr(channel, 0);

    if(!attr) {
        printf("failed to get attribute");
        return -1;
    }

    long long val = 0;
    int ret = iio_channel_attr_read_longlong(channel, attr, &val);

    if(ret < 0) {
        printf("failed to read attribute %d", -ret);
        return -1;
    }

    *value = val;
    return 0;
}

void func() {
    struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.20");

    if (!context) {
        printf("failed to get context\n");
        return;
    }

    int chanx_0, chanx_1;

    if (get_chan_raw(context, 0, &chanx_0) < 0 || get_chan_raw(context, 1, &chanx_1) < 0) {
        printf("failed to get X-axis values\n");
        return;
    }

    printf("Calibrating X-axis:\n");
    int calcX = 0;

    while (1) {
        printf("X axis chan0, chan1 val: %d, %d\n", chanx_0, chanx_1);

        if (chanx_0 > chanx_1 && chanx_0 - chanx_1 > 50) {
            printf("turn clockwise\n");
            calcX = 0;
        } else if (chanx_1 > chanx_0 && chanx_1 - chanx_0 > 50) {
            printf("turn counter clockwise\n");
            calcX = 0;
        } else {
            calcX++;
            if (calcX >= 5) {
                printf("X-axis calibrated!\n");
                break;
            }
        }

        if (get_chan_raw(context, 0, &chanx_0) < 0 || get_chan_raw(context, 1, &chanx_1) < 0) {
            printf("failed to get X-axis values\n");
            return;
        }

        usleep(500000);
    }

    int chany_2, chany_3;

    if (get_chan_raw(context, 2, &chany_2) < 0 || get_chan_raw(context, 3, &chany_3) < 0) {
        printf("failed to get Y-axis values\n");
        return;
    }

    printf("Calibrating Y-axis:\n");
    int calcY = 0;

    while (1) {
        if (chany_2 > chany_3 && chany_2 - chany_3 > 50) {
            printf("turn clockwise\n");
            calcY = 0;
        } else if (chany_3 > chany_2 && chany_3 - chany_2 > 50) {
            printf("turn counter clockwise\n");
            calcY = 0;
        } else {
            calcY++;
            if (calcY >= 5) {
                printf("Y-axis calibrated!\n");
                break;
            }
        }

        if (get_chan_raw(context, 2, &chany_2) < 0 || get_chan_raw(context, 3, &chany_3) < 0) {
            printf("failed to get Y-axis values\n");
            return;
        }

        usleep(500000);
    }

    int chanz_4, chanz_5;

    if (get_chan_raw(context, 4, &chanz_4) < 0 || get_chan_raw(context, 5, &chanz_5) < 0) {
        printf("failed to get Z-axis values\n");
        return;
    }

    printf("Calibrating Z-axis:\n");
    
    int calcZ = 0;

    while (1) {
        printf("Z axis chan4, chan5 val: %d, %d\n", chanz_4, chanz_5);

        if (chanz_4 > chanz_5 && ((chanz_4 - chanz_5 > 2048 && chanz_4 - chanz_5 - 2048 > 50) || (chanz_4 - chanz_5 < 2048 && 2048 - chanz_4 - chanz_5 > 50))) {
            printf("turn clockwise\n");
            calcZ = 0;
        } else if (chanz_5 > chanz_4 && ((chanz_5 - chanz_4 > 2048 && chanz_5 - chanz_4 - 2048 > 50) || (chanz_5 - chanz_4 < 2048 && 2048 - chanz_5 - chanz_4 > 50))) {
            printf("turn counter clockwise\n");
            calcZ = 0;
        } else {
            calcZ++;
            if (calcZ >= 5) {
                printf("Z-axis calibrated!\n");
                break;
            }
        }

        if (get_chan_raw(context, 4, &chanz_4) < 0 || get_chan_raw(context, 5, &chanz_5) < 0) {
            printf("failed to get Z-axis values\n");
            return;
        }

        usleep(500000);
    }
}

void buffer() {
    struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.20");

    if (!context) {
        printf("failed to get context\n");
        return;
    }

    struct iio_device *device = iio_context_find_device(context, "ad5592r_s");

    if(!device) {
        printf("failed to get device\n");
        return;
    }

    for(int i = 0; i < 6; i++) {
        struct iio_channel *channel = iio_device_get_channel(device, i);
        if(!channel) {
            printf("failed to get channel %d", i);
            return;
        }

        iio_channel_enable(channel);
    }

    int samples = 100;
    struct iio_buffer *buf = iio_device_create_buffer(device, samples, false);

    if(!buf) {
        printf("failed to get buffer\n");
        return;
    }

    // 2048 = 1G
    // magnitude of shock vector is sqrt(x^2 + y^2 + z^2)
    // shock threshold is 1.5G
    // use a while(true) to read buffers and detect shocks
    // after each iteration we need to do int ret = iio_buffer_refill(buf);

    // ex output:
    // detected shock: 3.2G
    // detected shock: 1.6G

    while(1) {
        int ret = iio_buffer_refill(buf);
        if(ret < 0) {
            printf("failed to refill buffer %d\n", -ret);
            iio_buffer_destroy(buf);
            iio_context_destroy(context);
            return;
        }

        void *start = iio_buffer_start(buf);
        void *end = iio_buffer_end(buf);
        ptrdiff_t step = iio_buffer_step(buf);

        for(void* p = start; p <= end ;p += step) {
			uint16_t x = ((uint16_t *)p)[0] - ((uint16_t *)p)[1];
			uint16_t y = ((uint16_t *)p)[2] - ((uint16_t *)p)[3];
			uint16_t z = ((uint16_t *)p)[4] - ((uint16_t *)p)[5];

			double mag = sqrt(x*x + y*y + z*z);

			if(mag / 2048. > 1.5) {
				printf("detected shock: %f\n", mag/2048.);
			}
		}

    }
}

        
        