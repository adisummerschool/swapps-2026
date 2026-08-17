#include "libfunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <unistd.h>


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