#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <time.h>
#include <unistd.h>

#define delta 50
#define Z_TARGET 1830

/*
itereaza prin fiecare aza (x y z) si asteapta pana cand axa e calibrata din potentiometru
outeti adauga un sleep intre citiri
daca sta 2 secunde(sau x citiri) calibrat atunci trecem la urmatoarea axa

ex output:
calibrate x axis:
turn clockwise!
turn clockwise!
turn clockwise!
turn clockwise!
calibrated!

calibrate y axis:
turn clockwise!
turn clockwise!
turn clockwise!
turn counterclockwise!
calibrated!

calibrate z axis:
calibrated!
*/


void func()
{
        struct iio_context *ctx = iio_create_context_from_uri("ip:10.76.84.14");
        if(!ctx) {
                printf("failed to get ctx\n");
                return;
        }

        struct iio_device *dev = iio_context_find_device(ctx, "iio_ad5592r_s");
                if(!dev) {
                printf("failed to get dev\n");
                return;
        }

        const char *channels[6] = {
                "voltage0",
                "voltage1",
                "voltage2",
                "voltage3",
                "voltage4",
                "voltage5",
        };
        
        printf("\n");
        long long raw = 0;

        for (int i = 0; i <=5; i++ ) {
                struct iio_channel *chn = iio_device_find_channel(dev, channels[i], false);
                if(!chn) {
                printf("failed to get chn\n");
                return;
                }

                int ret = iio_channel_attr_read_longlong(chn, "raw", &raw);
                if(ret < 0) {
                printf("failed to read ret %d\n", -ret);
                return;
                
                }
                printf("ch[%d]: %lld | ", i, raw);
        }
        printf("\n");


        // calibration
        for (int i = 0; i<=2; i++){
                int correct_readings = 0;

                switch (i) {
                        case 0: printf("\nCalibrating X axis \n"); break;
                        case 1: printf("\nCalibrating Y axis \n"); break;
                        case 2: printf("\nCalibrating Z axis \n"); break;
                        default: break;
                }

                while (correct_readings < 5)
                {
                        struct iio_channel *chn0 = iio_device_find_channel(dev, channels[i], false);
                        struct iio_channel *chn1 = iio_device_find_channel(dev, channels[i+2], false);
                        long long raw0 = 0, raw1 = 0;
                        int ret0 = iio_channel_attr_read_longlong(chn0, "raw", &raw0);
                        int ret1 = iio_channel_attr_read_longlong(chn1, "raw", &raw1);
                        long long aux = raw1 - raw0;
                        if (i == 2) aux = aux - Z_TARGET;
                        int stable = (aux < delta && aux > -delta) ? 1 : 0;
                        int sign_pos = (aux >= 0) ? 1 : 0;
                        
                        if (stable) {
                                correct_readings++;
                                if (correct_readings == 5) {
                                        printf("Calibrated! 5 correct readings\n");
                                } else {
                                        printf("[%d|%d]stable, wait\n", raw0, raw1);
                                }
                        }
                        else if (!stable && sign_pos) {
                                printf("[%lld|%lld]turn clockwise, deviation %lld \n", raw0, raw1, aux);
                                correct_readings = 0;
                        }
                        else if (!stable && !sign_pos) {
                                printf("[%lld|%lld]turn counterclockwise, deviation %lld \n", raw0, raw1, aux);
                                correct_readings = 0;
                        }

                        usleep(200000);
                }
        }
                        
}