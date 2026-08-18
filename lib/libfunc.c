#include "libfunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <unistd.h>
#include <math.h>


void func()
{
    int threshold = 50;
    int calibrated;
    long long val_channel0;
    long long val_channel1;
    int diff;

    struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.24");

    if(!context) {
        printf("failed to get context\n");
        return;
    }

    struct iio_device *device = iio_context_find_device(context, "ad5592r_s");

    if(!device) {
        printf("failed to get device\n");
        return;
    }

    struct iio_channel *channels[6];

    for(int i = 0; i < 6; i++) {
        channels[i] = iio_device_get_channel(device, i);

        if(!channels[i]) {
            printf("failed to get channel %d\n", i);
            return;
        }
    }

    for(int i = 0; i < 6; i += 2) {

        calibrated = 0;

        if(i == 0) {
            printf("\nCalibrate X axis:\n");
        } else if(i == 2) {
            printf("\nCalibrate Y axis:\n");
        } else {
            printf("\nCalibrate Z axis:\n");
        }

        do {
            const char *attr0 = iio_channel_get_attr(channels[i], 0);
            const char *attr1 = iio_channel_get_attr(channels[i + 1], 0);

            val_channel0 = 0;
            val_channel1 = 0;

            int ret = iio_channel_attr_read_longlong(channels[i], attr0, &val_channel0);

            if(ret < 0) {
                printf("failed to get val_channel0\n");
                return;
            }

            ret = iio_channel_attr_read_longlong(channels[i + 1], attr1, &val_channel1);

            if(ret < 0) {
                printf("failed to get val_channel1\n");
                return;
            }

            diff = val_channel0 - val_channel1;

            // x and y axis
            if(i < 4) {

                if(abs(diff) < threshold) {

                    if(i == 0) {
                        printf("Calibrated X axis\n");
                    } else {
                        printf("Calibrated Y axis\n");
                    }

                    calibrated = 1;

                } else {

                    if(diff > 0) {
                        printf("turn clockwise\n");
                    } else {
                        printf("turn counterclockwise\n");
                    }

                    calibrated = 0;
                }
            }

            // z axis
            else {

                if(val_channel1 < threshold &&
                   abs(val_channel0 - 2048) < threshold) {

                    printf("Calibrated Z axis\n");
                    calibrated = 1;

                } else {

                    if(diff > 0) {
                        printf("turn clockwise\n");
                    } else {
                        printf("turn counterclockwise\n");
                    }

                    calibrated = 0;
                }
            }

            sleep(1);

        } while(!calibrated);
    }
}

void buffer()
{
    struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.24");

    if(!context) {
        printf("failed to get context\n");
        return;
    }


    struct iio_device *device = iio_context_find_device(context, "ad5592r_s");

    if(!device) {
        printf("failed to get device\n");
        return;
    }

    struct iio_channel *ch0 = iio_device_get_channel(device, 0);
    if(!ch0) {
        printf("failed to get channel 0");
        return;
    }

    struct iio_channel *ch1 = iio_device_get_channel(device, 1);
    if(!ch1) {
        printf("failed to get channel 1");
        return;
    }

    struct iio_channel *ch2 = iio_device_get_channel(device, 2);
    if(!ch2) {
        printf("failed to get channel 2");
        return;
    }

    struct iio_channel *ch3 = iio_device_get_channel(device, 3);
    if(!ch3) {
        printf("failed to get channel 3");
        return;
    }

    struct iio_channel *ch4 = iio_device_get_channel(device, 4);
    if(!ch4) {
        printf("failed to get channel 4");
        return;
    }

    struct iio_channel *ch5 = iio_device_get_channel(device, 5);
    if(!ch5) {
        printf("failed to get channel 5");
        return;
    }

    iio_channel_enable(ch0);
    iio_channel_enable(ch1);
    iio_channel_enable(ch2);
    iio_channel_enable(ch3);
    iio_channel_enable(ch4);
    iio_channel_enable(ch5);


    int samples = 100;
    struct iio_buffer *buf = iio_device_create_buffer(device, samples, false);
    if(!buf) {
        printf("failed to get buffer\n");
        return;
    }


    while(true) {
        int ret = iio_buffer_refill(buf);
        if(ret < 0) {
                printf("failed to refill buffer %d\n", -ret);
                iio_buffer_destroy(buf);
                iio_context_destroy(context);
                break;
        }

        void *start = iio_buffer_start(buf);

        void *end = iio_buffer_end(buf);

        ptrdiff_t step = iio_buffer_step(buf);

        for(void *b = start; b < end; b += step) {
            uint16_t val_x0 = ((uint16_t *)b)[0];
            uint16_t val_x1 = ((uint16_t *)b)[1];
            uint32_t val_x = val_x1 + val_x0;

            uint16_t val_y0 = ((uint16_t *)b)[2];
            uint16_t val_y1 = ((uint16_t *)b)[3];
            uint32_t val_y = val_y1 + val_y0;

            uint16_t val_z0 = ((uint16_t *)b)[4];
            uint16_t val_z1 = ((uint16_t *)b)[5];
            uint32_t val_z = val_z1 + val_z0;

            float magnitude = sqrt(val_x * val_x + val_y * val_y + val_z * val_z);
            float threshold = magnitude / 2048.0;

            if(threshold > 1.5 || threshold < 0.5) {
                printf("detected shock: %.2f G\n", threshold);
            }
        }
    }

    int ret = iio_buffer_refill(buf);

    iio_buffer_destroy(buf);
    iio_context_destroy(context);
}