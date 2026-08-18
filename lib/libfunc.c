#include "libfunc.h"
#include <stdio.h>
#include <stdlib.h>     
#include <unistd.h>   
#include <iio.h>
#include <math.h>
#include <stdbool.h>

int get_ch_raw(char *ch_name, struct iio_device *device) {
    struct iio_channel *chan = iio_device_find_channel(device, ch_name, false);
    if(!chan) {
        printf("Unable to find channel %s\n", ch_name);
        return -1;
    }

    long long value = 0;
    int ret = iio_channel_attr_read_longlong(chan, "raw", &value);
    if(ret < 0) {
        printf("Unable to read attribute: %d\n", -ret);
        return -1;
    }

    return (int)value;
}

void func(){
    struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.15");
    if(!context) {
        printf("Unable to create IIO context\n");
        return;
    }

    struct iio_device *device = iio_context_find_device(context, "ad5592r_s");
    if(!device) {
        printf("Unable to find device\n");
        return;
    }

    char *axes[] = {"x", "y", "z"};
    char *channels[3][2] = { {"voltage0", "voltage1"}, {"voltage2", "voltage3"}, {"voltage4", "voltage5"} };

    int z_value = 2048; 

    for(int i=0; i<3; i++){
        printf("calibrating %s axis: \n", axes[i]);

        int cnt = 0;
        
        while(cnt < 20){
            int val1 = get_ch_raw(channels[i][0], device);
            int val2 = get_ch_raw(channels[i][1], device); 

            int diff = val1 - val2;
            int target = (i == 2) ? z_value : 0;
            
            int error = diff - target;
            int delta = abs(error);

            if(delta < 50){
                cnt++;
            }
            else {
                cnt = 0;
                
                if(error > 0){
                    printf("turn clockwise\n");
                }
                else{
                    printf("turn counterclockwise\n");
                }
            }

            usleep(100000); 
        }
        printf("Calibrated!\n\n");
    }
}

  /*
    - get buf end
    - get buf step
    - iterate through buf, convert data, print ch0 samples
    - destroy buffer
    */

    /*
    - 1G = 2048
    - magnitude = sqrt(x^2 + y^2 + z^2)
    - shock threshold = 1.5G
    - use a while(true) to read buffers and detect shocks
    - after each iteration we need to do int ret = iio_buffer_refill(buf) 

    ex output:
    detected shock: 3.2G
    detected shock: 1.6G
    */

void buffer(){
    struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.15");
    if(!context) {
        printf("Unable to create IIO context\n");
        return;
    }

    struct iio_device *device = iio_context_find_device(context, "ad5592r_s");
    if(!device) {
        printf("Unable to find device\n");
        iio_context_destroy(context);
        return;
    }

    struct iio_channel *channels[6] = {0};
    for(int i = 0; i < 6; i++) {
        channels[i] = iio_device_get_channel(device, i);
        if(!channels[i]) {
            printf("Unable to find channel %d\n", i);
            iio_context_destroy(context);
            return;
        }
        iio_channel_enable(channels[i]);
    }

    int samples = 100;
    struct iio_buffer* buf = iio_device_create_buffer(device, samples, false);
    if(!buf) {
        printf("Unable to create buffer\n");
        iio_context_destroy(context);
        return;
    }
   
    double shock_threshold = 1.5 * 2048; 

    while(true){
        int ret = iio_buffer_refill(buf);
        if(ret < 0) {
            printf("Unable to refill buffer: %d\n", -ret);
            break;
        }

        void *start = iio_buffer_start(buf);
        void *end = iio_buffer_end(buf);
        ptrdiff_t step = iio_buffer_step(buf);

        for (void *i = start; i < end; i += step) {
            int16_t data[6] = {0};

            for(int k = 0; k < 6; k++) {
                iio_channel_convert(channels[k], &data[k], i + 2 * k);
            }

            int x = (int)data[0] - (int)data[1];
            int y = (int)data[2] - (int)data[3];
            int z = (int)data[4] - (int)data[5];           

            double magnitude = sqrt(x*x + y*y + z*z);

            if (magnitude > shock_threshold) {
                printf("detected shock: %.1fG\n", magnitude / 2048.0);
            }
            
        }
    }

    iio_buffer_destroy(buf);
    iio_context_destroy(context);
}


