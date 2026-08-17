#include <stdio.h>
#include <iio.h>
#include <stdlib.h>

int main(){

const char *uri = "ip:10.76.84.5";
const char *device_name = "iio_adc";

    struct iio_context *ctx = iio_create_context_from_uri(uri);
    if (!ctx) {
        perror("Eroare la crearea contextului IIO");
    }

    struct iio_device *dev = iio_context_find_device(ctx, device_name);
    if (!dev) {
        fprintf(stderr, "Dispozitivul '%s' nu a fost găsit!\n", device_name);
        iio_context_destroy(ctx);
    }

    struct iio_channel *chn = iio_device_find_channel(dev, "voltage0", false);
    if (!chn) {
        fprintf(stderr, "Canalul 'voltage0' nu a fost găsit!\n");
        iio_context_destroy(ctx);
    }

    long long raw_val = 0;
    int err = iio_channel_attr_read_longlong(chn, "raw", &raw_val);

    if (err < 0) {
        fprintf(stderr, "Eroare la citirea atributului 'raw': cod %d\n", err);
        iio_context_destroy(ctx);
    }

    printf("Valoarea raw: %lld\n", raw_val);

    iio_context_destroy(ctx);

}

//printf("Hello world\n");
//printf("%d\n", iio_get_backends_count());




