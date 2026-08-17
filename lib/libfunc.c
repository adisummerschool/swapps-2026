#include "libfunc.h"
#include <stdio.h>
#include <iio.h>

void func()
{
                struct iio_context *ctx;
                struct iio_device *dev;
                struct iio_channel *chn[6];
                long long raw_val;
                char chn_name[20];
                ctx = iio_create_context_from_uri("ip:10.76.84.250");
                if(!ctx)
                        {
                                printf("Failed to get CTX");
                                return;
                        }
                dev = iio_context_find_device(ctx, "ad5592r_s");
                if(!dev)
                        {
                                printf("Failed to get dev");
                                return;
                        }
                for(int i = 0; i < 6; i++)
                        {
                                snprintf(chn_name, sizeof(chn_name), "voltage%d", i);
                                chn[i] = iio_device_find_channel(dev, chn_name, false);
                                if(!chn[i])
                                        {
                                                printf("Failed to get channel %d",i);
                                                return;
                                        }
                                if (chn[i] != NULL) 
                                        {
                                                int valoare = iio_channel_attr_read_longlong(chn[i], "raw", &raw_val);
                                                if(valoare<0)
                                                        {
                                                                printf("Failed to read attribute: %d", -valoare);
                                                                return;
                                                        }
                                                printf("Raw %s value: %lld\n", chn_name, raw_val);
                                        } 
                                else 
                                        {
                                                printf("Nu am putut gasi canalul %s\n", chn_name);
                                        }
                        }
                iio_context_destroy(ctx);


}
