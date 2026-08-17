#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <unistd.h> 

void func()
{
        struct iio_context *ctx;
        struct iio_device *dev;
        struct iio_channel *chn[6];
        long long raw_val;
        int cx = 0, cy = 0, cz = 0;
        char chn_name[20];
        
        ctx = iio_create_context_from_uri("ip:10.76.84.250");
        if(!ctx)
                {
                        printf("Failed to get CTX\n");
                        return;
                }
        
        dev = iio_context_find_device(ctx, "ad5592r_s");
        if(!dev)
                {
                        printf("Failed to get dev\n");
                        return;
                }
        
        // Initializare canale
        for(int i = 0; i < 6; i++)
                {
                        snprintf(chn_name, sizeof(chn_name), "voltage%d", i);
                        chn[i] = iio_device_find_channel(dev, chn_name, false);
                        if(!chn[i])
                                {
                                        printf("Failed to get channel %d\n", i);
                                        return;
                                }
                }

        long long raw_val_neg, raw_val_poz;
        int tacturi;
        while(!cx)
                {
                        iio_channel_attr_read_longlong(chn[0], "raw", &raw_val_neg);
                        iio_channel_attr_read_longlong(chn[1], "raw", &raw_val_poz);
                        
                        if(raw_val_neg < 220 || raw_val_neg > 300) 
                        {
                                printf("\r[V0: %lld | V1: %lld] Calibrate x axis: turn clockwise\033[K", raw_val_neg, raw_val_poz);
                        }
                        else if(raw_val_poz < 220 || raw_val_poz > 300) 
                        {
                                printf("\r[V0: %lld | V1: %lld] Calibrate x axis: turn anticlockwise\033[K", raw_val_neg, raw_val_poz);
                        }
                        else {
                                tacturi = 0;
                                while(tacturi < 5) {
                                        printf("\r[V0: %lld | V1: %lld] X Axis stabilizare %d/5...\033[K", raw_val_neg, raw_val_poz, tacturi + 1);
                                        usleep(10000); // 100ms
                                        
                                        iio_channel_attr_read_longlong(chn[0], "raw", &raw_val_neg);
                                        iio_channel_attr_read_longlong(chn[1], "raw", &raw_val_poz);
                                        
                                        if(raw_val_neg < 220 || raw_val_neg > 300 || raw_val_poz < 220 || raw_val_poz > 300) {
                                                break; 
                                        }
                                        tacturi++;
                                }
                                
                                if(tacturi == 5) {
                                        printf("\r[V0: %lld | V1: %lld] X Axis Calibrated!\033[K\n", raw_val_neg, raw_val_poz);
                                        cx = 1; 
                                }
                        }
                        usleep(10000);
                }
        while(!cy)
                {
                        iio_channel_attr_read_longlong(chn[2], "raw", &raw_val_neg);
                        iio_channel_attr_read_longlong(chn[3], "raw", &raw_val_poz);
                        
                        if(raw_val_neg < 220 || raw_val_neg > 300) 
                                {
                                        printf("\r[V2: %lld | V3: %lld] Calibrate y axis: turn clockwise\033[K", raw_val_neg, raw_val_poz);
                                }
                        else if(raw_val_poz < 220 || raw_val_poz > 300) 
                                {
                                        printf("\r[V2: %lld | V3: %lld] Calibrate y axis: turn anticlockwise\033[K", raw_val_neg, raw_val_poz);
                                }
                        else 
                                {
                                        tacturi = 0;
                                        while(tacturi < 5) 
                                        {
                                                printf("\r[V2: %lld | V3: %lld] Y Axis stabilizare %d/5...\033[K", raw_val_neg, raw_val_poz, tacturi + 1);
                                                usleep(10000);
                                                
                                                iio_channel_attr_read_longlong(chn[2], "raw", &raw_val_neg);
                                                iio_channel_attr_read_longlong(chn[3], "raw", &raw_val_poz);
                                                
                                                if(raw_val_neg < 220 || raw_val_neg > 300 || raw_val_poz < 220 || raw_val_poz > 300) 
                                                {
                                                        break;
                                                }
                                                tacturi++;
                                        }
                                        
                                        if(tacturi == 5) 
                                                {
                                                        printf("\r[V2: %lld | V3: %lld] Y Axis Calibrated!\033[K\n", raw_val_neg, raw_val_poz);
                                                        cy = 1;
                                                }
                                }
                        usleep(10000);
                }
        while(!cz)
        {
                iio_channel_attr_read_longlong(chn[4], "raw", &raw_val_neg);
                iio_channel_attr_read_longlong(chn[5], "raw", &raw_val_poz);
                
                if(raw_val_neg < 2000 || raw_val_neg > 2100) 
                        {
                                printf("\r[V4: %lld | V5: %lld] Calibrate z axis: turn clockwise\033[K", raw_val_neg, raw_val_poz);
                        }
                else if(raw_val_poz < 0 || raw_val_poz > 100) 
                        {
                                printf("\r[V4: %lld | V5: %lld] Calibrate z axis: turn anticlockwise\033[K", raw_val_neg, raw_val_poz);
                        }
                else 
                        {
                                tacturi = 0;
                                while(tacturi < 5) 
                                        {
                                                printf("\r[V4: %lld | V5: %lld] Z Axis stabilizare %d/5...\033[K", raw_val_neg, raw_val_poz, tacturi + 1);
                                                usleep(10000);
                                                
                                                iio_channel_attr_read_longlong(chn[4], "raw", &raw_val_neg);
                                                iio_channel_attr_read_longlong(chn[5], "raw", &raw_val_poz);
                                                
                                                if(raw_val_neg < 2000 || raw_val_neg > 2100 || raw_val_poz < 0 || raw_val_poz > 100) 
                                                        {
                                                                break;
                                                        }
                                                tacturi++;
                                        }
                                
                                if(tacturi == 5) 
                                        {
                                                printf("\r[V4: %lld | V5: %lld] Z Axis Calibrated!\033[K\n", raw_val_neg, raw_val_poz);
                                                cz = 1;
                                        }
                        }
                usleep(10000);
        }
        printf("\nX, Y, Z successfully calibrated!\n");
        iio_context_destroy(ctx);
}