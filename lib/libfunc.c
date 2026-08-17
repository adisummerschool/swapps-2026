#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <unistd.h>
void func()
{
    long long dif;

    struct iio_context *my_dev = iio_create_context_from_uri("ip:10.76.84.113");
    if(!my_dev) {
        printf("failed to get context");
        return;
    }

    struct iio_device *my_dev_name = iio_context_find_device(my_dev, "ad5592r_s");
    if(!my_dev_name) {
        printf("failed to get device");
        return;
    }

    struct iio_channel *ch0 = iio_device_get_channel(my_dev_name, 0);
    if(!ch0) {
        printf("failed to get channel\n");
        return;
    }

    struct iio_channel *ch1 = iio_device_get_channel(my_dev_name, 1);
    if(!ch1) {
        printf("failed to get channel\n");
        return;
    }

    struct iio_channel *ch2 = iio_device_get_channel(my_dev_name, 2);
    if(!ch2) {
        printf("failed to get channel\n");
        return;
    }

    struct iio_channel *ch3 = iio_device_get_channel(my_dev_name, 3);
    if(!ch3) {
        printf("failed to get channel\n");
        return;
    }

    struct iio_channel *ch4 = iio_device_get_channel(my_dev_name, 4);
    if(!ch4) {
        printf("failed to get channel\n");
        return;
    }

    struct iio_channel *ch5 = iio_device_get_channel(my_dev_name, 5);
    if(!ch5) {
        printf("failed to get channel\n");
        return;
    }

    printf("Clibrate x-axis:\n");

    do {
        const char *x0 = iio_channel_get_attr(ch0, 0);
        if(!x0) {
            printf("failed to get attribute");
            return;
        }

        const char *x1 = iio_channel_get_attr(ch1, 0);
        if(!x1) {
            printf("failed to get attribute");
            return;
        }

        long long val_x0 = 0;
        long long val_x1 = 0;
        int ret_x0 = iio_channel_attr_read_longlong(ch0, x0, &val_x0);
        int ret_x1 = iio_channel_attr_read_longlong(ch1, x1, &val_x1);
        if(ret_x0 < 0 || ret_x1 < 0)  {
            printf("failed to read attribute %d", -ret_x1);
            return;
        }


        dif = val_x0-val_x1;

        if(val_x1<val_x0){
            // turn clockwise / counterclockwise
            printf("turn clockwise: \n", &val_x0, &val_x1);
        }

        else if(val_x1>val_x0) {
            printf("turn counterclockwise\n");
        }

        sleep(1);

    } while(abs(dif)>50);
    printf("calibrated x-axis!\n");

    dif = 0;
    printf("Clibrate y-axis:\n");
    do {
        const char *x2 = iio_channel_get_attr(ch2, 0);
        if(!x2) {
            printf("failed to get attribute");
            return;
        }

        const char *x3 = iio_channel_get_attr(ch3, 0);
        if(!x3) {
            printf("failed to get attribute");
            return;
        }

        long long val_x2 = 0;
        long long val_x3 = 0;
        int ret_x2 = iio_channel_attr_read_longlong(ch2, x2, &val_x2);
        int ret_x3 = iio_channel_attr_read_longlong(ch3, x3, &val_x3);
        if(ret_x2 < 0 || ret_x3 < 0)  {
            printf("failed to read attribute %d", -ret_x3);
            return;
        }

        dif = val_x3-val_x2;

        if(val_x3<val_x2){
            // turn clockwise / counterclockwise
            printf("turn clockwise\n");
        }

        else
            printf("turn counterclockwise\n");

        sleep(1);

    } while(abs(dif)>50);

    printf("calibrated y-axis!\n");

    dif = 0;

    long long v1, v2;

    printf("Clibrate z-axis:\n");
    do {
        const char *x4 = iio_channel_get_attr(ch4, 0);
        if(!x4) {
            printf("failed to get attribute");
            return;
        }

        const char *x5 = iio_channel_get_attr(ch5, 0);
        if(!x5) {
            printf("failed to get attribute");
            return;
        }

        long long val_x4 = 0;
        long long val_x5 = 0;
        int ret_x4 = iio_channel_attr_read_longlong(ch4, x4, &val_x4);
        int ret_x5 = iio_channel_attr_read_longlong(ch5, x5, &val_x5);
        if(ret_x4 < 0 || ret_x5 < 0)  {
            printf("failed to read attribute %d", -ret_x4);
            return;
        }

        if(val_x4>val_x5){
            // turn clockwise / counterclockwise
            printf("turn clockwise\n");
        }

        else
            printf("turn counterclockwise\n");


        v1 = val_x4;
        v2 = val_x5;

    } while(v2 > 50 || abs(v1 - 2048) > 50);
    printf("calibrated z-axis!\n");
}