#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

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

        /*
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
        */
                        
}

void buffer()
{
            /*
        get buf end
        get buf step
        iterate through buf, convert data, print ch0 samples
        destroy buffer
        */

        struct iio_context *ctx = iio_create_context_from_uri("ip:10.76.84.14");
        if(!ctx) {
                printf("failed to get ctx\n");
                return;
        }

        struct iio_device *dev = iio_context_find_device(ctx, "iio_ad5592r_s");
        if(!dev) {
                printf("failed to get dev\n");
                iio_context_destroy(ctx);
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

        struct iio_channel *chn[6];
        for (int i = 0; i<=5; i++) {
                chn[i] = iio_device_find_channel(dev, channels[i], false);
                if (!chn[i]) {
                        printf("failed to get chn[%d]\n", i);
                        iio_context_destroy(ctx);
                        return;
                }
                iio_channel_enable(chn[i]);
        }

        int samples = 100;
        struct iio_buffer *buf = iio_device_create_buffer(dev, samples, false);
        if(!buf) {
                printf("failed to get buffer\n");
                iio_context_destroy(ctx);
                return;
        }

        int ret = iio_buffer_refill(buf);
        if(ret < 0){
                printf("failed to refill buffer %d\n", -ret);
                iio_buffer_destroy(buf);
                iio_context_destroy(ctx);
                return;
        }


        while (true) {
                void *start = iio_buffer_start(buf);
                void *end   = iio_buffer_end(buf);
                ptrdiff_t step = iio_buffer_step(buf);
                float max_magnitude = 0.0;
                float mag = 0.0;

                for (uint8_t *ptr = start; (void*)ptr < end; ptr += step) {
                        uint16_t *value = (uint16_t *)ptr;
                        uint16_t Xp = value[0], Yp = value[1], Zp = value[2];
                        uint16_t Xn = value[3], Yn = value[4], Zn = value[5];

                        float Xg = (Xp - Xn) / 2048.0f;
                        float Yg = (Yp - Yn) / 2048.0f;
                        float Zg = (Zp - Zn) / 2048.0f;

                        mag = sqrtf(Xg*Xg + Yg*Yg + Zg*Zg);
                        if (mag > max_magnitude) max_magnitude = mag;
                }

                if (max_magnitude > 1.5f) {   // shock threshold
                        printf("detected shock %.2fG\n", max_magnitude);
                }

                int ret = iio_buffer_refill(buf);
                if(ret < 0){
                        printf("failed to refill buffer %d\n", -ret);
                        iio_buffer_destroy(buf);
                        iio_context_destroy(ctx);
                        return;
                }
        }
        
        /*
        adapt for 6 channels
        2048 = 1G, max 2G
        detect shocks from one buffer read (sample size)
        magnitudinea vactorului de soc = sqrt(x^2 + y^2 + z^2)
        shock threshhold 1.5G

        use a while(true) to read buffers and detect shocks (like wrapping all I guess)
        after each iteration we need to do int ret = iio_buffer_refill(buf);
        reduce redundant displays
        ex:
        detected shock 3.2G
        detected shock 1.6G
        */

        iio_buffer_destroy(buf);
        iio_context_destroy(ctx);
        return;
}