#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <unistd.h> 
#include <stdint.h> 
#include <stddef.h> 

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




void buffer()
{
        struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.250");
        if(!cont)
        {
                printf("failed to get ctx\n");
                return;
        }

        struct iio_device *dev = iio_context_find_device(cont, "ad5592r_s");
        if(!dev) 
        {
                printf("failed to get device\n");
                iio_context_destroy(cont);
                return;
        }       

        struct iio_channel *ch0 = iio_device_find_channel(dev, "voltage0", false);
        if(!ch0)
        {
                printf("failed to get ch0\n");
                iio_context_destroy(cont);
                return;
        }
        
        // Activați canalul înainte de a crea buffer-ul
        iio_channel_enable(ch0);

        // --- SETAREA ȘI PORNIREA TRIGGER-ULUI PENTRU A EVITA EROAREA 110 ---
        struct iio_device *trigger = NULL;
        for (unsigned int i = 0; i < iio_context_get_devices_count(cont); i++) {
                struct iio_device *t = iio_context_get_device(cont, i);
                if (iio_device_is_trigger(t)) {
                        trigger = t;
                        break; 
                }
        }
        
        if (!trigger) {
                printf("Nu am gasit niciun trigger disponibil pe placa!\n");
                iio_context_destroy(cont);
                return;
        }
        
        // Asociem trigger-ul găsit device-ului nostru
        iio_device_set_trigger(dev, trigger);

        // Pornim trigger-ul setându-i o frecvență (ex: 100 de eșantioane pe secundă)
        if (iio_device_find_attr(trigger, "sampling_frequency")) {
                iio_device_attr_write_longlong(trigger, "sampling_frequency", 100);
        }
        // -------------------------------------------------------------------

        int samples = 100;
        struct iio_buffer *buf = iio_device_create_buffer(dev, samples, false);
        if(!buf)
        {
                printf("failed to get buffer\n");
                iio_context_destroy(cont);
                return;
        } 
        
        int ret = iio_buffer_refill(buf);
        if(ret < 0)
        {
                printf("failed to refill buffer %d\n", -ret);
                iio_buffer_destroy(buf);
                iio_context_destroy(cont);
                return; 
        }

        void *start = iio_buffer_start(buf);
        void *end = iio_buffer_end(buf);
        ptrdiff_t step = iio_buffer_step(buf);
        
        //printf("Reading %d samples from CH0...\n", samples);
        
        int sample_count = 0;
        for (void *ptr = start; ptr < end; ptr += step) 
        {
                uint16_t raw_val;
                
                // iio_channel_convert extrage datele specifice canalului din tot pachetul
                iio_channel_convert(ch0, &raw_val, ptr); 
                
                printf("%u\n", raw_val);
                sample_count++;
        }
        
        iio_buffer_destroy(buf);
        iio_context_destroy(cont);
}