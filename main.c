#include <stdio.h>
#include <iio.h>


//TASK
//read iio.h(see slack) and find a way to read the raw attribute from the voltage0 channel from <name device> by using ur board
//URI and libiio functions (no buffer yet). 
//Print the raw values taht was read to stdout(prinft) and you can go home

//PS compile code is 'gcc -o main main.c -liio' 
int main()
        {
                struct iio_context *ctx;
                struct iio_device *dev;
                struct iio_channel *chn[6];
                long long raw_val;
                char chn_name[20];
                ctx = iio_create_context_from_uri("ip:10.76.84.250");
                dev = iio_context_find_device(ctx, "ad5592r_s");
                for(int i = 0; i < 6; i++)
                        {
                                snprintf(chn_name, sizeof(chn_name), "voltage%d", i);
                                chn[i] = iio_device_find_channel(dev, chn_name, false);
                                if (chn[i] != NULL)
                                        {
                                                iio_channel_attr_read_longlong(chn[i], "raw", &raw_val);
                                                printf("Raw %s value: %lld\n", chn_name, raw_val);
                                        }
                                else
                                        {
                                                printf("Nu am putut gasi canalul %s\n", chn_name);
                                        }
                        }
                iio_context_destroy(ctx);

                return 0;
        }