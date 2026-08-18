#include "libfunc.h"
#include <stdio.h>
#include <iio.h>
#include <math.h>

/*

itereaza prin fiecare axa (x, y, z) si asteapta pan acand axa e
calibrata din potentiometru
. Puteti adauga un sleep intre citiri
. Daca sta 2secunde (sau x citiri) calibrat atunci trecem la urmatoare axa
. puteti face o functie: int get_ch_raw(<ch_name>);
. o axa contine 2 canale si trebuie comparat valorile de pe ele
. use a threshold (placa are noise); delta < 10
. no buffers

ex output:
calibrate x axis:
turn clockwise
turn clockwise
turn clockwise
turn clockwise
turn clockwise
Calibrated!

calibrate y axis:
turn clockwise
turn clockwise
turn counterclockwise
turn counterclockwise
Calibrated



calibrate z axis:
turn clockwise
turn clockwise
turn counterclockwise
turn counterclockwise
Calibrated!!!!!



*/

#include<unistd.h>


void buffer()
{
    struct iio_context *cont;
    struct iio_device *dev;
    struct iio_channel *channels[6];

    cont = iio_create_context_from_uri("ip:10.76.84.137");
    if (!cont) {
        printf("failed to get ctx\n");
        return;
    }

    dev = iio_context_find_device(cont, "ad5592r_s");
    if (!dev) {
        printf("failed to get dev\n");
        iio_context_destroy(cont);
        return;
    }

    for (int i = 0; i < 6; i++) {
        channels[i] = iio_device_get_channel(dev, i);

        if (!channels[i]) {
            printf("failed to get ch %d\n", i);
            iio_context_destroy(cont);
            return;
        }

        iio_channel_enable(channels[i]);
    }

    int samples = 100;

    struct iio_buffer *buf =
        iio_device_create_buffer(dev, samples, false);

    if (!buf) {
        printf("failed to get buffer\n");
        iio_context_destroy(cont);
        return;
    }

	while (true) {

    int ret = iio_buffer_refill(buf);

    if (ret < 0) {
        printf("failed to refill buffer %d\n", -ret);
        break;
    }

    void *end = iio_buffer_end(buf);
    ptrdiff_t step = iio_buffer_step(buf);

    void *ptr[6];

    for (int i = 0; i < 6; i++) {
        ptr[i] = iio_buffer_first(buf, channels[i]);
    }

    while (ptr[0] < end) {

        long long value[6] = {0};

        for (int i = 0; i < 6; i++) {
            iio_channel_convert(channels[i], &value[i], ptr[i]);
        }

        double x = (double)(value[0] - value[1]) / 2048.0;
        double y = (double)(value[2] - value[3]) / 2048.0;
        double z = (double)(value[4] - value[5]) / 2048.0;

        double magnitude = sqrt(
            x * x +
            y * y +
            z * z
        );

        if (magnitude > 1.5) {
            printf("detected shock: %.2fG\n", magnitude);
        }

        for (int i = 0; i < 6; i++) {
            ptr[i] += step;
        }
    }
}

	//look in iio.h

	// get buf end
	// get buf step
	// iterate through buf, convert data, print ch0 samples
	//destroy buffer

    

	// 2048 = 1G
	// magnitudinea vectorului de soc = sqrt(x^2 + y^2 + z^2)
	// shock threshold = 1.5G
	// use a while(true) to read buffers and detect shocks
	// after each itereation we need to do int = iio_buffer_refill(buf)

	// ex output:
	// detected shock: 3.2G
	// detected shock: 1.6G

    iio_buffer_destroy(buf);
    iio_context_destroy(cont);
}


void func ()
{
	/*int threshold = 50;

    struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.137");
	if(!cont) {
		printf("failed to get ctx");
		return;
	}

	struct iio_device *dev = iio_context_find_device(cont, "ad5592r_s");
	if(!dev) {
		printf("failed to get dev");
		return;
	}



	struct iio_channel *channels[6];



	for(int i=0; i<6; i++) {
		struct iio_channel *ch = iio_device_get_channel(dev, i);
		if(!ch) {
			printf("failed to get ch %d", i);
			return;
		}

		channels[i] = ch;
	}

	int calibrated;
	for (int i=0; i<6; i+=2){
		calibrated = 0;
		do {
			const char *x1 = iio_channel_get_attr(channels[i], 0);

			long long val1 = 0;
			int ret = iio_channel_attr_read_longlong(channels[i], x1, &val1);
			if(0>ret) {
				printf("failed to get val1");
				return;
			}
			const char *x2 = iio_channel_get_attr(channels[i+1], 0);

			long long val2 = 0;
			ret = iio_channel_attr_read_longlong(channels[i+1], x2, &val2);
			if(0>ret) {
				printf("failed to get val2");
				return;
			}

			if(i<4) {
				if(abs(val1-val2) < threshold) {
					printf("Calibrated %s axis\n", i<2 ? "X" : "Y");
					calibrated = 1;
				} else {
					printf("spin %s; %d - %d\n", val1 > val2 ? "clockwaise" : "counterclockwaise", val1, val2);
					calibrated = 0;
				}
			} else {
				if(val2 < threshold && (abs(val1 - 2048) < threshold)) {
					printf("Calibrated Z axis\n");
					calibrated = 1;
				} else {
					printf("spin %s; %d - %d\n", val1 > val2 ? "clockwaise" : "counterclockwaise", val1, val2);
					calibrated = 0;
				}
			}

			sleep(1);

		} while(!calibrated);
}*/
	buffer();
}

