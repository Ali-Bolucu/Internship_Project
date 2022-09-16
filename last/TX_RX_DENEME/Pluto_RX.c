#include"Pluto.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#include <stdio.h>
#define _USE_MATH_DEFINES // for C
#include <math.h>
#include <iio.h>


extern struct iio_device* tx;
extern struct iio_context* ctx;
extern struct iio_device* dev;

extern struct iio_channel* rx0_i;
extern struct iio_channel* rx0_q;
extern struct iio_channel* tx0_i;
extern struct iio_channel* tx0_q;   
extern struct iio_buffer* rxbuf;
extern struct iio_buffer* txbuf;
extern struct iio_channel* chad;


extern int buffer_size;
extern int sample_rate;

// For RX, not using
void rx_freq(float freq) {
	/**
	* TX : altvoltage1
	* RX : altvoltage0
	**/
	printf("Freq is changing to: %lf\n", freq);

	dev = iio_context_find_device(ctx, "ad9361-phy");

	iio_channel_attr_write_longlong(
		iio_device_find_channel(dev, "altvoltage0", true),
		"frequency",
		freq);
}