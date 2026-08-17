#include "stdio.h"
#include <iio.h>

///TODO: sa creem un context
///sa facem un attr in cod si sa citim de la device0 toti parametrii 

void print_value_raw(){
        struct iio_context *st = iio_create_network_context("10.76.84.210");
        printf("%d\n", iio_context_get_devices_count(st));

        for(int i = 0; i <= 5; i++){
                struct iio_device *devices = iio_context_get_device(st, 0);
                struct iio_channel *chan = iio_device_get_channel(devices, i);
                long long data = 0;
                iio_channel_attr_read_longlong(chan, "raw", &data);
                printf("%ld ", data);
        }
}

int main(){

        printf("Hello world!\n");

        printf("%d\n", iio_get_backends_count());

        print_value_raw();

        return 0;
}

