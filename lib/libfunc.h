#ifndef LIB_H
#define LIB_H

void func();
void calibrate_axis(struct iio_channel *chan_pos, struct iio_channel *chan_neg);
void calibrate_z_axis(struct iio_channel *chan_pos, struct iio_channel *chan_neg);
void calibrate_ad5592r_s();

#endif //LIB_H
