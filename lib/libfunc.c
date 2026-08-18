#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <unistd.h>
#include <math.h>

/*
void func()
{
        struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.70");
        if(!cont) {
                printf("failed to get ctx");
                return;
        }

        struct iio_device *dev = iio_context_find_device(cont, "iio_ad5592r_s");
        if(!dev) {
                printf("failed to get dev");
                return;
        }

        struct iio_channel *chan0 = iio_device_get_channel(dev, 0);
        if(!chan0) {
                printf("failed to get chan");
                return;
        }

        const char *attr = iio_channel_get_attr(chan0, 0);
        if(!attr) {
                printf("failed to get attr");
                return;
        }

        long long val = 0;
        int ret = iio_channel_attr_read_longlong(chan0, attr, &val);
        if(ret < 0) {
                printf("failed to get attr %d", -ret);
                return;
        }

        printf("%lld\n", val);

}
*/

void calibrateX()
{
        struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.70");
        if(!cont) {
                printf("failed to get ctx");
                return;
        }

        struct iio_device *dev = iio_context_find_device(cont, "iio_ad5592r_s");
        if(!dev) {
                printf("failed to get dev");
                return;
        }

        long long val1 = 0;
        long long val2 = 0;

        printf("Calibrate X axis:\n");

        do {
                struct iio_channel *chan0 = iio_device_get_channel(dev, 0);
                if(!chan0) {
                printf("failed to get chan0");
                return;
                }

                struct iio_channel *chan1 = iio_device_get_channel(dev, 1);
                if(!chan1) {
                printf("failed to get chan1");
                return;
                }

                const char *attr0 = iio_channel_get_attr(chan0, 0);
                if(!attr0) {
                printf("failed to get attr0");
                return;
                }

                const char *attr1 = iio_channel_get_attr(chan1, 0);
                if(!attr1) {
                printf("failed to get attr1");
                return;
                }

                int ret1 = iio_channel_attr_read_longlong(chan0, attr0, &val1);
                if(ret1 < 0) {
                printf("failed to get attr0 %d", -ret1);
                return;
                }

                int ret2 = iio_channel_attr_read_longlong(chan1, attr1, &val2);
                if(ret2 < 0) {
                printf("failed to get attr1 %d", -ret2);
                return;
                }

                if (val1 > val2) {
                        printf("Turn clockwise\n");
                }
                else {
                        printf("Turn counterclockwise\n");
                }
                sleep(1);
        }
        while (abs(val1 - val2) < 50);

        printf("Calibrated!\n");

        iio_context_destroy(cont);

}

void calibrateY()
{
        struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.70");
        if(!cont) {
                printf("failed to get ctx");
                return;
        }

        struct iio_device *dev = iio_context_find_device(cont, "iio_ad5592r_s");
        if(!dev) {
                printf("failed to get dev");
                return;
        }

        long long val1 = 0;
        long long val2 = 0;

        printf("Calibrate Y axis:\n");

        do {
                struct iio_channel *chan2 = iio_device_get_channel(dev, 2);
                if(!chan2) {
                printf("failed to get chan2");
                return;
                }

                struct iio_channel *chan3 = iio_device_get_channel(dev, 3);
                if(!chan3) {
                printf("failed to get chan3");
                return;
                }

                const char *attr2 = iio_channel_get_attr(chan2, 0);
                if(!attr2) {
                printf("failed to get attr2");
                return;
                }

                const char *attr3 = iio_channel_get_attr(chan3, 0);
                if(!attr3) {
                printf("failed to get attr3");
                return;
                }

                int ret1 = iio_channel_attr_read_longlong(chan2, attr2, &val1);
                if(ret1 < 0) {
                printf("failed to get attr2 %d", -ret1);
                return;
                }

                int ret2 = iio_channel_attr_read_longlong(chan3, attr3, &val2);
                if(ret2 < 0) {
                printf("failed to get attr3 %d", -ret2);
                return;
                }

                if (val1 > val2) {
                        printf("Turn clockwise\n");
                }
                else {
                        printf("Turn counterclockwise\n");
                }
                sleep(1);
        }
        while (abs(val1 - val2) < 50);

        printf("Calibrated!\n");

        iio_context_destroy(cont);

}

void calibrateZ()
{
        struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.70");
        if(!cont) {
                printf("failed to get ctx");
                return;
        }

        struct iio_device *dev = iio_context_find_device(cont, "iio_ad5592r_s");
        if(!dev) {
                printf("failed to get dev");
                return;
        }

        long long val1 = 0;
        long long val2 = 0;

        printf("Calibrate Z axis:\n");

        do {
                struct iio_channel *chan4 = iio_device_get_channel(dev, 4);
                if(!chan4) {
                printf("failed to get chan4");
                return;
                }

                struct iio_channel *chan5 = iio_device_get_channel(dev, 5);
                if(!chan5) {
                printf("failed to get chan5");
                return;
                }

                const char *attr4 = iio_channel_get_attr(chan4, 0);
                if(!attr4) {
                printf("failed to get attr4");
                return;
                }

                const char *attr5 = iio_channel_get_attr(chan5, 0);
                if(!attr5) {
                printf("failed to get attr5");
                return;
                }

                int ret1 = iio_channel_attr_read_longlong(chan4, attr4, &val1);
                if(ret1 < 0) {
                printf("failed to get attr4 %d", -ret1);
                return;
                }

                int ret2 = iio_channel_attr_read_longlong(chan5, attr5, &val2);
                if(ret2 < 0) {
                printf("failed to get attr5 %d", -ret2);
                return;
                }

                if (val1 > val2) {
                        printf("Turn clockwise\n");
                }
                else {
                        printf("Turn counterclockwise\n");
                }
                sleep(1);
        }
        while ((val2 < 50) || (abs(val1 - val2) < 50));

        printf("Calibrated!\n");

        iio_context_destroy(cont);

}

void buffer()
{
        struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.70");
        if(!cont) {
                printf("failed to get ctx");
                return;
        }

        struct iio_device *dev = iio_context_find_device(cont, "iio_ad5592r_s");
        if(!dev) {
                printf("failed to get dev");
                return;
        }

        struct iio_channel *chan0 = iio_device_get_channel(dev, 0);
        if(!chan0) {
                printf("failed to get chan0");
                return;
        }

        struct iio_channel *chan1 = iio_device_get_channel(dev, 1);
        if(!chan1) {
                printf("failed to get chan1");
                return;
        }

        struct iio_channel *chan2 = iio_device_get_channel(dev, 2);
        if(!chan2) {
                printf("failed to get chan2");
                return;
        }

        struct iio_channel *chan3 = iio_device_get_channel(dev, 3);
        if(!chan3) {
                printf("failed to get chan3");
                return;
        }

        struct iio_channel *chan4 = iio_device_get_channel(dev, 4);
        if(!chan4) {
                printf("failed to get chan4");
                return;
        }

        struct iio_channel *chan5 = iio_device_get_channel(dev, 5);
        if(!chan5) {
                printf("failed to get chan5");
                return;
        }

        iio_channel_enable(chan0);
        iio_channel_enable(chan1);
        iio_channel_enable(chan2);
        iio_channel_enable(chan3);
        iio_channel_enable(chan4);
        iio_channel_enable(chan5);

        int samples = 100;
        struct iio_buffer *buf = iio_device_create_buffer(dev, samples, false);
        if(!buf) {
                printf("failed to get buffer");
                return;
        }

        while(true) {
                int ret = iio_buffer_refill(buf);
                if(ret < 0) {
                        printf("failed to refill buffer %d\n", -ret);
                        iio_buffer_destroy(buf);
                        iio_context_destroy(cont);
                        return;
                }

                void *start = iio_buffer_start(buf);

                void *end = iio_buffer_end(buf);

                ptrdiff_t step = iio_buffer_step(buf);

                for(void *i = start; i <= end; i += step) {
                        uint16_t val_x0 = ((uint16_t *)i)[0];
                        uint16_t val_x1 = ((uint16_t *)i)[1];
                        uint32_t val_x = val_x0 + val_x1;

                        uint16_t val_y0 = ((uint16_t *)i)[2];
                        uint16_t val_y1 = ((uint16_t *)i)[3];
                        uint32_t val_y = val_y0 + val_y1;

                        uint16_t val_z0 = ((uint16_t *)i)[4];
                        uint16_t val_z1 = ((uint16_t *)i)[5];
                        uint32_t val_z = val_z0 + val_z1;

                        float magnitude = sqrt(val_x * val_x + val_y * val_y + val_z * val_z);
                        float threshold = magnitude / 2048.0;

                        if (threshold > 1.5) {
                                printf("detected shock: %.2f G\n", threshold);
                        }
                }
        }

        int ret = iio_buffer_refill(buf);

        iio_buffer_destroy(buf);
        iio_context_destroy(cont);
}