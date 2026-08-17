#include "libfunc.h"
#include "stdio.h"
#include <iio.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

/*
TODO: itereaza prin fiecare axa (x, y, z) si asteapta pana cand axa e calibrata
din potentiometru
Daca sta 2 secunde (sau x citiri) calirat atunci trecem la urmatorea axa
O axa contine 2 canale si trebuie comparate valorile de pe ele
Use a trashold
no buffer


ex output:
calibrate x axis:
turn clockwise
turn clockwise
turn clockwise
turn clockwise
Calibrate!

calibrate y axis:
turn clockwise
turn clockwise
turn counterclockwise
turn counterclockwise
turn clockwise
Calibrate!

calibrate z axis:
turn clockwise
turn clockwise
turn counterclockwise
turn counterclockwise
turn clockwise
Calibrate!

*/

void calibrate_function()
{
        int cont = 0;
        const int TH_CALIBRARE = 30;
        const int TH_FINISH_CALIB = 10;
        for (;;)
        {
                system("clear");
                printf("Calibrate X axis\n");
                long long val_m = 0, val_p = 0;
                get_val_axis('x', &val_m, &val_p);

                if (abs(val_p - val_m) < TH_CALIBRARE)
                {
                        cont++;
                        if (cont > TH_FINISH_CALIB)
                        {
                                printf("Calibrate X finish!\n\n");
                                break;
                        }
                }
                else
                {
                        cont = 0;
                        if ((val_p - val_m) > TH_FINISH_CALIB)
                        {
                                printf("turn clockwise\n");
                        }
                        else
                        {
                                printf("turn counterclockwise\n");
                        }
                }
                sleep(2);
        }

        cont = 0;

        for (;;)
        {
                system("clear");
                printf("Calibrate Y axis\n");
                long long val_m = 0, val_p = 0;
                get_val_axis('y', &val_m, &val_p);

                if (abs(val_p - val_m) < TH_CALIBRARE)
                {
                        cont++;
                        if (cont > TH_FINISH_CALIB)
                        {
                                printf("Calibrate Y finish!\n\n");
                                break;
                        }
                }
                else
                {
                        cont = 0;
                        if ((val_p - val_m) > TH_FINISH_CALIB)
                        {
                                printf("turn clockwise\n");
                        }
                        else
                        {
                                printf("turn counterclockwise\n");
                        }
                }
                sleep(2);
        }

        cont = 0;

        const int TH_CALIBRATE_Z_N_Mi = 0;
        const int TH_CALIBRATE_Z_N_Ma = 40;
        const int TH_CALIBRATE_Z_P_Mi = 2020;
        const int TH_CALIBRATE_Z_P_Ma = 2060;

        for (;;)
        {
                system("clear");
                printf("Calibrate Z axis\n");
                long long val_m = 0, val_p = 0;
                get_val_axis('z', &val_m, &val_p);

                if (val_p >= TH_CALIBRATE_Z_P_Mi && val_p <= TH_CALIBRATE_Z_P_Ma)
                {

                        if (val_m >= TH_CALIBRATE_Z_N_Mi && val_m <= TH_CALIBRATE_Z_N_Ma)
                        {
                                cont++;
                                if (cont > TH_FINISH_CALIB)
                                {
                                        printf("Calibrate Z finish!\n\n");
                                        break;
                                }
                        }
                }
                else
                {
                        cont = 0;
                        if ((val_p - val_m) > TH_FINISH_CALIB)
                        {
                                printf("turn clockwise\n");
                        }
                        else
                        {
                                printf("turn counterclockwise\n");
                        }
                }
                sleep(2);
        }
        system("clear");
        printf("Calibrate successfuly!!!");
}

void get_val_axis(char numb_axis, long long *val_m, long long *val_p)
{
        struct iio_context *st = iio_create_network_context("10.76.84.210");
        if (!st)
        {
                printf("faild to get context");
                return;
        }
        // printf("%d\n", iio_context_get_devices_count(st));

        struct iio_device *devices = iio_context_get_device(st, 0);
        if (!devices)
        {
                printf("failed to get device");
                return;
        }
        numb_axis = tolower(numb_axis);

        switch (numb_axis)
        {
        case 'x':
        {
                struct iio_channel *chan = iio_device_get_channel(devices, 0);
                if (!devices)
                {
                        printf("failed to get channel");
                        return;
                }
                long long data = 0;
                int ret = iio_channel_attr_read_longlong(chan, "raw", &data);
                if (ret < 0)
                {
                        printf("failed to get data %d\n", -ret);
                        return;
                }
                printf("Minus value: %ld\n", data);
                *val_m = data;

                chan = iio_device_get_channel(devices, 1);
                if (!devices)
                {
                        printf("failed to get channel");
                        return;
                }
                data = 0;
                ret = iio_channel_attr_read_longlong(chan, "raw", &data);
                if (ret < 0)
                {
                        printf("failed to get data %d\n", -ret);
                        return;
                }
                printf("Plus value: %ld\n", data);
                *val_p = data;

                break;
        }
        case 'y':
        {
                struct iio_channel *chan = iio_device_get_channel(devices, 2);
                if (!devices)
                {
                        printf("failed to get channel");
                        return;
                }
                long long data = 0;
                int ret = iio_channel_attr_read_longlong(chan, "raw", &data);
                if (ret < 0)
                {
                        printf("failed to get data %d\n", -ret);
                        return;
                }
                printf("Minus value: %ld\n", data);
                *val_m = data;

                chan = iio_device_get_channel(devices, 3);
                if (!devices)
                {
                        printf("failed to get channel");
                        return;
                }
                data = 0;
                ret = iio_channel_attr_read_longlong(chan, "raw", &data);
                if (ret < 0)
                {
                        printf("failed to get data %d\n", -ret);
                        return;
                }
                printf("Plus value: %ld\n", data);
                *val_p = data;

                break;
        }
        case 'z':
        {
                struct iio_channel *chan = iio_device_get_channel(devices, 5);
                if (!devices)
                {
                        printf("failed to get channel");
                        return;
                }
                long long data = 0;
                int ret = iio_channel_attr_read_longlong(chan, "raw", &data);
                if (ret < 0)
                {
                        printf("failed to get data %d\n", -ret);
                        return;
                }
                printf("Minus value: %ld\n", data);
                *val_m = data;

                chan = iio_device_get_channel(devices, 4);
                if (!devices)
                {
                        printf("failed to get channel");
                        return;
                }
                data = 0;
                ret = iio_channel_attr_read_longlong(chan, "raw", &data);
                if (ret < 0)
                {
                        printf("failed to get data %d\n", -ret);
                        return;
                }
                printf("Plus value: %ld\n", data);
                *val_p = data;

                break;
        }
        default:
                break;
        }
}

void func()
{
        struct iio_context *st = iio_create_network_context("10.76.84.210");
        if (!st)
        {
                printf("faild to get context");
                return;
        }
        printf("%d\n", iio_context_get_devices_count(st));

        for (int i = 0; i <= 5; i++)
        {
                struct iio_device *devices = iio_context_get_device(st, 0);
                if (!devices)
                {
                        printf("failed to get device");
                        return;
                }
                struct iio_channel *chan = iio_device_get_channel(devices, i);
                if (!devices)
                {
                        printf("failed to get channel");
                        return;
                }
                long long data = 0;
                int ret = iio_channel_attr_read_longlong(chan, "raw", &data);
                if (ret < 0)
                {
                        printf("failed to get data %d\n", -ret);
                        return;
                }
                printf("%ld ", data);
        }
}
