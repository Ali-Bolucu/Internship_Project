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


float freq_save_for_clear;

float sin_i[24800];
float cos_q[24800];

extern int buffer_size;
extern int sample_rate;

 //wr_ch_str(chn, "gain_control_mode", "manual" );

// For TX
bool tx_lo_freq(double freq) {
	/**
	* TX : altvoltage1
	* RX : altvoltage0
	**/
	printf("Freq is changing to: %lf\n", freq);
	
	if (!ctx){return false;}

	dev = iio_context_find_device(ctx, "ad9361-phy");
	
	if (!dev){return false;}

	iio_channel_attr_write_longlong(
		iio_device_find_channel(dev, "altvoltage1", true),
		"frequency",
		freq);
		
	return true;
}


bool tx_sampling_freq(double freq) {

	iio_channel_attr_write_longlong(
	iio_device_find_channel(dev, "voltage0", true),
	"sampling_frequency",
	freq);


	return true;
}


bool tx_bandwidth_freq(double freq) {

	iio_channel_attr_write_longlong(
	iio_device_find_channel(dev, "voltage0", true),
	"rf_bandwidth",
	freq);

	return true;
}

bool tx_port(char* port) {

	iio_channel_attr_write(
	iio_device_find_channel(dev, "voltage0", true),
	"rf_port_select",
	port);


	return true;
}



long long freq_check(){

	long long freq;


	iio_channel_attr_write_longlong(
		iio_device_find_channel(dev, "altvoltage1", true),
		"frequency",
		freq);


	return freq;
}



bool clear() {
	
	printf("* Destroying buffers\n");
	if (txbuf) { iio_buffer_destroy(txbuf); }
	
	txbuf = iio_device_create_buffer(tx, buffer_size, true);

	if (!txbuf) { return false; }


	sin_gen(freq_save_for_clear);

	return true;
}


bool gain_tx(double gain) {

	chad = iio_device_find_channel(dev, "voltage0", true);
	iio_channel_attr_write_double(chad, "hardwaregain", gain);

	return true;
}



bool Multi(float freq_sin, float num) {


	printf("* Destroying buffers\n");

	if (txbuf) { iio_buffer_destroy(txbuf); }

	txbuf = iio_device_create_buffer(tx, buffer_size, true);
	if (!txbuf) { return false; }

	for (int i = 0; i < 24800; i++) {
		sin_i[i] += sin(((float)i * M_PI * 2 * freq_sin) / (float)(sample_rate)) *num ;
		cos_q[i] += cos(((float)i * M_PI * 2 * freq_sin) / (float)(sample_rate)) *num;

	}



	char* p_dat, * p_end;
	ptrdiff_t p_inc;

	p_inc = iio_buffer_step(txbuf);
	p_end = iio_buffer_end(txbuf);
	int iter = 0;
	for (p_dat = iio_buffer_first(txbuf, tx0_q); p_dat < p_end; p_dat += p_inc) {
		((int16_t*)p_dat)[0] = cos_q[iter] * 16384;
		((int16_t*)p_dat)[1] = sin_i[iter++] * 16384;

	}

	iio_buffer_push(txbuf);

 return true;
}




/* signal generator: make a sine wave */
bool sin_gen(float freq_sin) {
	
	freq_save_for_clear = freq_sin;
	
	printf("* Destroying buffers\n");
	if (txbuf) { iio_buffer_destroy(txbuf); }
	
	txbuf = iio_device_create_buffer(tx, buffer_size, true);
	
	
	for (int i = 0; i < 24800; i++) {
		sin_i[i] = sin(((float)i * M_PI * 2 * freq_sin) / (float)(sample_rate));
		cos_q[i] = cos(((float)i * M_PI * 2 * freq_sin) / (float)(sample_rate));

	}


	char* p_dat, * p_end;
	ptrdiff_t p_inc;

	p_inc = iio_buffer_step(txbuf);
	p_end = iio_buffer_end(txbuf);
	int iter = 0;
	for (p_dat = iio_buffer_first(txbuf, tx0_q); p_dat < p_end; p_dat += p_inc) {
		((int16_t*)p_dat)[0] = cos_q[iter] * 16384;
		((int16_t*)p_dat)[1] = sin_i[iter++] * 16384;

	}

	iio_buffer_push(txbuf);
	return true;
}

/* cleanup and exit */
bool shutdown_pluto()
{
	printf("* Destroying buffers\n");
	if (rxbuf) { iio_buffer_destroy(rxbuf); }
	if (txbuf) { iio_buffer_destroy(txbuf); }

	printf("* Disabling streaming channels\n");
	if (rx0_i) { iio_channel_disable(rx0_i); }
	if (rx0_q) { iio_channel_disable(rx0_q); }
	if (tx0_i) { iio_channel_disable(tx0_i); }
	if (tx0_q) { iio_channel_disable(tx0_q); }

	printf("* Destroying context\n");
	if (ctx) { iio_context_destroy(ctx); }
	return true;
}