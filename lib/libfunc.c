#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include "libfunc.h"

#define THR 25
#define SH_THR 3072

void calibrate()
{
    struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.220");
    if(!cont) {
        printf("failed to get ctx\n");
        return;
    }

    struct iio_device *dev = iio_context_find_device(cont, "ad5592r_s");
    if(!dev) {
        printf("failed to get dev\n");
        return;
    }

    const char *axis_names[] = {"X", "Y", "Z"};

    for (int i = 0; i < 3; i++) {
        bool cal = false;
        int valid_reads = 0; 

        printf("\n--- Begin calibration for %s ---\n", axis_names[i]);

        struct iio_channel *cp = iio_device_get_channel(dev, i * 2);
        struct iio_channel *cn = iio_device_get_channel(dev, i * 2 + 1);

        if(!cp || !cn) {
            printf("Failed to get channels for %s axis.\n", axis_names[i]);
            return;
        }
        
        const char *attr_p = iio_channel_get_attr(cp, 0);
        const char *attr_n = iio_channel_get_attr(cn, 0);

        if(!attr_p || !attr_n){
            printf("Failed to get attributes for %s axis.\n", axis_names[i]);
            return;
        }

        while(!cal) {
            long long val_p = 0;
            long long val_n = 0;
            
            int ret_p = iio_channel_attr_read_longlong(cp, attr_p, &val_p);
            int ret_n = iio_channel_attr_read_longlong(cn, attr_n, &val_n);
            
            if(ret_p < 0 || ret_n < 0){
                printf("Failed to read values for %s axis.\n", axis_names[i]);
                return;
            }
            
            if (i == 2) { 
                long long diff = val_p - 2048;

                if (llabs(diff) < 10) { 
                    valid_reads++;
                    if (valid_reads >= 3) {
                        printf("%s axis calibrated!\n", axis_names[i]);
                        cal = true;
                    } else {
                        printf("Hold still...\n", valid_reads);
                        sleep(1);
                    }
                } else {
                    valid_reads = 0; 
                    if (val_p < 2048) {
                        printf("Turn clockwise \n");
                    } else {
                        printf("Turn counterclockwise \n");
                    }
                    printf("Positive_Value %lld (Target: 2048)\n", val_p);
                    sleep(1);
                }
            } 
           
            else { 
                long long diff = val_p - val_n;

                if (llabs(diff) < 10) {
                    valid_reads++;
                    if (valid_reads >= 3) {
                        printf("%s axis calibrated!\n", axis_names[i]);
                        cal = true;
                    } else {
                        printf("Hold still...\n", valid_reads);
                        sleep(1);
                    }
                } else {
                    valid_reads = 0; 
                    if (val_p < val_n) {
                        printf("Turn clockwise \n");
                    } else {
                        printf("Turn counterclockwise \n");
                    }
                    printf("Positive_Value %lld , Negative_Value %lld \n", val_p, val_n);
                    sleep(1);
                }
            }
        }
    }
    
    printf("\nAxis Calibrated Succesfully!\n");
}

void func ()
{
    struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.220");
    if(!cont) {
        printf("failed to get ctx\n");
        return;
    }

    struct iio_device *dev = iio_context_find_device(cont, "ad5592r_s");
    if(!dev) {
        printf("failed to get dev\n");
        return;
    }

    struct iio_channel *chan0 = iio_device_get_channel(dev, 0);
    if(!chan0) {
        printf("failed to get chn\n");
        return;
    }
    
    const char *attr = iio_channel_get_attr(chan0, 0);
    if(!attr) {
        printf("failed to get attr\n");
        return;
    }

    long long val = 0;
    int ret = iio_channel_attr_read_longlong(chan0, attr, &val);
    if(ret < 0) {
        printf("failed to get attr %d\n", -ret);
        return;
    }
    printf("%lld\n", val);
}


void buffer() {
  struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.220");
	if (!cont) {
    printf("Failed to get ctx\n");
    return;
  }

  struct iio_device *dev = iio_context_find_device(cont, "ad5592r_s");
		if (!dev) {
    printf("Failed to get dev\n");
    return;
  }

  struct iio_channel *chan[6];
  for(int i = 0; i < 6; ++i) {
    chan[i] = iio_device_get_channel(dev, i);
		if (!chan[i]) {
      printf("Failed to get chan %d.\n", i);
      return;
    }
    iio_channel_enable(chan[i]);
  }

  int samples = 100;
  struct iio_buffer *buf = iio_device_create_buffer(dev, samples, false);

  if (!buf) {
    printf("Failed to get buffer.\n");
    return;
  }


  double g_x = 0.0, g_y = 0.0, g_z = 0.0;
  const double ALPHA = 0.9; 

  for(;;){
    int ret = iio_buffer_refill(buf);
    if (ret < 0) {
      printf("buf read failed with err code %d.\n", -ret);
      return;
    }

    void *start = iio_buffer_start(buf);
    void *end = iio_buffer_end(buf);
    ptrdiff_t step = iio_buffer_step(buf);
    
    uint16_t val[6] = {0, 0, 0, 0, 0, 0};

    for(void *ptr = start; ptr < end; ptr += step) {
      iio_channel_convert(chan[0], &val[0], ptr);
      iio_channel_convert(chan[1], &val[1], ptr + sizeof(uint16_t));
      iio_channel_convert(chan[2], &val[2], ptr + 2 * sizeof(uint16_t));
      iio_channel_convert(chan[3], &val[3], ptr + 3 * sizeof(uint16_t));
      iio_channel_convert(chan[4], &val[4], ptr + 4 * sizeof(uint16_t));
      iio_channel_convert(chan[5], &val[5], ptr + 5 * sizeof(uint16_t));

      int32_t ax = (int32_t)val[0] - (int32_t)val[1];
      int32_t ay = (int32_t)val[2] - (int32_t)val[3];
      int32_t az = (int32_t)val[4] - (int32_t)val[5];

      g_x = ALPHA * g_x + (1.0 - ALPHA) * (double)ax;
      g_y = ALPHA * g_y + (1.0 - ALPHA) * (double)ay;
      g_z = ALPHA * g_z + (1.0 - ALPHA) * (double)az;

      double shock_x = (double)ax - g_x;
      double shock_y = (double)ay - g_y;
      double shock_z = (double)az - g_z;

      double sh_vec = sqrt(shock_x * shock_x + shock_y * shock_y + shock_z * shock_z);

      if (sh_vec > SH_THR){
          printf("SHOCK DETECTED: %.1f G\n", sh_vec / 2048.0);
      }
    }
  }
  iio_buffer_destroy(buf);
  iio_context_destroy(cont);
}