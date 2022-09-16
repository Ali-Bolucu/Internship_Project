#pragma once


#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdbool.h>

bool pluto(void);
bool sin_gen(float freq_sin);
bool shutdown_pluto(void);


//TX
bool tx_lo_freq(double freq);
bool tx_sampling_freq(double freq);
bool tx_bandwidth_freq(double freq);
bool tx_port(char* port);
long long freq_check(void);

bool Multi(float freq_sin, float num);
bool clear(void);
bool gain_tx(double gain);


//RX
void rx_freq(float freq);

#endif

