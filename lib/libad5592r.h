#ifndef LIBAD5592R_H
#define LIBAD5592R_H

void calibrate(const char *ip);
int read_ad5592r(const char *ip);

#endif // LIBAD5592R_H