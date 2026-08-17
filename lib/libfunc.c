#include "libfunc.h"
#include <stdio.h>
#include <iio.h>

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



void func ()
{
	int threshold = 50;

    struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.34");
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
}
}

void buffer()
{
    struct iio_context *cont = iio_create_context_from_uri("ip:10.76.84.34");
	if(!cont) {
		printf("failed to get ctx\n");
		return;
	}

	struct iio_device *dev = iio_context_find_device(cont, "ad5592r_s");
	if(!dev) {
		printf("failed to get dev\n");
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
        iio_channel_enable(channels[i]);
	}

    int samples = 100;
    struct iio_buffer* buf = iio_device_create_buffer(dev, samples, false);
    if(!buf){
        printf("filed to get buffer\n");
        return;
    }

    int ret = iio_buffer_refill(buf);
    if(ret<0){
        printf("filed to refill buffer %d\n", -ret);
        return;
    }

    void *start = iio_buffer_start(buf);
    void *end = iio_buffer_end(buf);
	ptrdiff_t step = iio_buffer_step(buf); //distanta dintre 2 pointeri
 
	for (void *ptr = start; ptr < end; ptr += step) {
		int16_t sample = 0;
		iio_channel_convert(channels[0], &sample, ptr);
		printf("%d\n", sample);
	}
 
	iio_buffer_destroy(buf);

}