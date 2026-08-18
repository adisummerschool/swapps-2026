#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <unistd.h>
#include <math.h>

long long func(int channel) {
    struct iio_context* cont = iio_create_context_from_uri("ip:10.76.84.31");
    if (!cont) {
        printf("Failed to get context");
        return -1;
    }
    struct iio_device* dev = iio_context_find_device(cont, "ad5592r_s");
    if (!dev) {
        printf("Failed to get device");
        return -1;
    }
    struct iio_channel *chan = iio_device_get_channel(dev, channel);
    if (!chan) {
        printf("Failed to get channel %d", channel);
        return -1;
    }
    const char *attr = iio_channel_get_attr(chan, 0);
    if (!attr) {
        printf("Failed to get attribute");
        return -1;
    }
    long long val = 0;
    int ret = iio_channel_attr_read_longlong(chan, attr, &val);
    if (ret < 0) {
        printf("Failed to get value from attr %d", -ret);
        return -1;
    }
    //printf("%lld\n", val);
    return val;
}

bool z_axis(int value1, int value2) {
    double average_value1 = value1 / (double)4095;
    double average_value2 = value2 / (double)4095;
    if ((average_value1 >= 0.45 && average_value1 <= 0.55) ||
         average_value2 >= 0.45 && average_value2 <= 0.55) {
        return true;
    }
    return false;
}

void calibrate_axis(int channel1, int channel2, char axis_name, int threshold) {
    bool calibrated = false;
    int readings = 0;
    while (true) {
        long long diff = abs(func(channel1) - func(channel2));
        if (!calibrated) {
            if (diff <= threshold || z_axis(func(channel1), func(channel2))) {
                readings++;
                printf("Holding calibration for %d seconds..\n", readings);
                if (readings >= 3) {
                    printf("%c axis is calibrated! Moving on..\n", axis_name);
                    calibrated = true;
                }
                continue;
            }
            if (diff > threshold || !z_axis(func(channel1), func(channel2))) {
                printf("Calibrate %c further more! %c: %d - %c: %d\n", axis_name, axis_name, func(channel1), axis_name, func(channel2));
                if (func(channel1) < func(channel2)) {
                    printf("Turn the potentiometer LEFT!\n");
                } else {
                    printf("Turn the %c potentiometer RIGHT!\n", axis_name);
                }
            }
        } else {
            break;
        }
        sleep(1);
    }
}

void calibrate() {
    printf("Starting calibrating...\n");
    sleep(1);
    calibrate_axis(0, 1, 'X', 10);
    calibrate_axis(2, 3, 'Y', 10);
    calibrate_axis(4, 5, 'Z', 10);
    printf("Board is fully calibrated!");
}

void buffer() {
    struct iio_context* cont = iio_create_context_from_uri("ip:10.76.84.31");
    if (!cont) {
        printf("Failed to get context");
        return;
    }
    struct iio_device* dev = iio_context_find_device(cont, "ad5592r_s");
    if (!dev) {
        printf("Failed to get device");
        return;
    }
    struct iio_channel *chan[6];
    for (int i = 0; i < 6; i++) {
        chan[i] = iio_device_get_channel(dev, i);
        if (!chan[i]) {
            printf("Failed to get channel %d", i);
            return;
        }
        iio_channel_enable(chan[i]);
    }
    
    int samples = 100;
    struct iio_buffer* buf = iio_device_create_buffer(dev, samples, false);
    if (!buf) {
        printf("Failed to get buffer!");
        return;
    }
    int ret = iio_buffer_refill(buf);
    if (ret < 0) {
        printf("Failed to refill buffer with error: %d", ret);
        return;
    }

    void *start = iio_buffer_start(buf);
    long long int val[6] = {0, 0, 0, 0, 0, 0};
    int i = 0;
    double alpha = 0.9, g_x = 0, g_y = 0, g_z = 0;
    while (true) {
        for (void* ptr = start; ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf)) {
            iio_channel_convert(chan[0], &val[0], ptr);
            iio_channel_convert(chan[1], &val[1], ptr + 2);
            iio_channel_convert(chan[2], &val[2], ptr + 4);
            iio_channel_convert(chan[3], &val[3], ptr + 6);
            iio_channel_convert(chan[4], &val[4], ptr + 8);
            iio_channel_convert(chan[5], &val[5], ptr + 10);

            int ax = val[0] - val[1];
            int ay = val[2] - val[3];
            int az = val[4] - val[5];

            g_x = alpha * g_x + (1.0 - alpha) * (double)ax;
            g_y = alpha * g_y + (1.0 - alpha) * (double)ay;
            g_z = alpha * g_z + (1.0 - alpha) * (double)az;

            double shock_x = (double)ax - g_x;
            double shock_y = (double)ay - g_y;
            double shock_z = (double)az - g_z;

            double shock = sqrt(shock_x * shock_x + shock_y * shock_y + shock_z * shock_z);
            if (shock > 3072) {
                printf("Detected shock %.2fG\n", shock / 2048);
            }

        }
        int ret = iio_buffer_refill(buf);
    }
    iio_buffer_destroy(buf);
}