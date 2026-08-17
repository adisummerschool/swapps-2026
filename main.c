#include <stdio.h>
#include <iio.h>


//TASK
//read iio.h(see slack) and find a way to read the raw attribute from the voltage0 channel from <name device> by using ur board
//URI and libiio functions (no buffer yet). 
//Print the raw values taht was read to stdout(prinft) and you can go home

//PS compile code is 'gcc -o main main.c -liio' 
int main ()  {
        printf("hello, world!\n");
        printf("%d\n", iio_get_backends_count());

        struct iio_context *context = iio_create_context_from_uri("ip:10.76.84.250");
        struct iio_device *device = iio_context_find_device(context, "ad5592r_s");
        struct iio_channel *channel_0 = iio_device_get_channel(device, 0);
        const char *attr = iio_channel_get_attr(channel_0, 0);
        long long value = 0;
        iio_channel_attr_read_longlong(channel_0, attr, &value);

        printf("%lld\n", value);

        return 0;
}