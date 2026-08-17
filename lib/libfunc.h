#ifndef FUNC_H
#define FUNC_H
#include <iio.h>

int get_chan_raw(struct iio_context *context, int channel_num, int *value);
void func();

#endif  // FUNC_H