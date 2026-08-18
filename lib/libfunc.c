#include "libfunc.h"
#include "stdio.h"
#include <iio.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

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

void buffer()
{
        struct iio_context *st = iio_create_network_context("10.76.84.210");
        if (!st)
        {
                printf("faild to get context\n");
                return;
        }

        struct iio_device *devices = iio_context_get_device(st, 0);
        if (!devices)
        {
                printf("failed to get device\n");
                return;
        }

        for (int i = 0; i < 6; i++)
        {
                struct iio_channel *chn = iio_device_get_channel(devices, i);
                if (!chn)
                {
                        printf("failed to get channel\n");
                        return;
                }
                iio_channel_enable(chn);
        }

        int samples = 100;
        struct iio_buffer *buf = iio_device_create_buffer(devices, samples, false);
        if (!buf)
        {
                printf("failed to get buffer\n");
                return;
        }
        int start_cont = 0;
        for (;;)
        {
                
                size_t ret = iio_buffer_refill(buf); // avem date in buffer
                if (ret < 0)
                {
                        printf("failed to refill buffer %d\n", -ret);
                        return;
                }
                printf("Sample buffer nr: %d\n", start_cont++);
                void *start = iio_buffer_start(buf); // start contine punctul de start din buffer
                if (!start)
                {
                        printf("buffer allocation failed\n");
                        return;
                }
                uint16_t val_data_axis[7][110];
                memset(val_data_axis, 0, sizeof(val_data_axis));
                for (int i = 0; i < 6; i++)
                {
                        struct iio_channel *chn = iio_device_get_channel(devices, i);
                        if (!chn)
                        {
                                printf("failed to get channel\n");
                                return;
                        }
                        // 2048 = 1G
                        int k = 0;
                        for (void *ptr = iio_buffer_first(buf, chn); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
                        {
                                uint16_t val_data = 0;
                                if (!ptr)
                                {
                                        printf("failed to get data from buffer\n");
                                        return;
                                }
                                iio_channel_convert(chn, &val_data, ptr);
                                // printf("%d\n", val_data);
                                val_data_axis[i][k++] = val_data;
                        }
                        // break;
                }

                double v_soc[110];
                memset(v_soc, 0.0, sizeof(v_soc));
                for (int i = 0; i < 100; i++)
                {
                        v_soc[i] = sqrt((val_data_axis[0][i] - val_data_axis[1][i]) *
                                            (val_data_axis[0][i] - val_data_axis[1][i]) +
                                        (val_data_axis[2][i] - val_data_axis[3][i]) *
                                            (val_data_axis[2][i] - val_data_axis[3][i]) +
                                        (val_data_axis[4][i] - val_data_axis[5][i]) *
                                            (val_data_axis[4][i] - val_data_axis[5][i])) /
                                   2048;
                        // printf("%f ", v_soc[i]);
                }
                // printf("\n");
                const double SHOCK_TH = 0.5;
                for (int i = 10; i < 100; i += 10)
                {
                        if (fabs(v_soc[i - 10] - v_soc[i]) > SHOCK_TH)
                        {
                                printf("Shock detected - Magnitude %lf\n", fabs(v_soc[i - 10] - v_soc[i]));
                                break;
                        }
                }
        }

        iio_buffer_destroy(buf);
        iio_context_destroy(st);
}
