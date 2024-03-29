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


struct iio_device* tx;
struct iio_context* ctx;
struct iio_device* dev;

struct iio_channel* rx0_i;
struct iio_channel* rx0_q;
struct iio_channel* tx0_i;
struct iio_channel* tx0_q;   
struct iio_buffer* rxbuf;
struct iio_buffer* txbuf;
struct iio_channel* chad;
bool stop;


/* helper macros */
#define MHZ(x) ((long long)(x*1000000.0 + .5))
#define GHZ(x) ((long long)(x*1000000000.0 + .5))

#pragma warning (disable : 4996)

int buffer_size = 24800;
int sample_rate = 3072000;

static char tmpstr[64];

/* RX is input, TX is output */
enum iodev { RX, TX };

/* common RX and TX streaming params */
struct stream_cfg {
	long long bw_hz; // Analog banwidth in Hz
	long long fs_hz; // Baseband sample rate in Hz
	long long lo_hz; // Local oscillator frequency in Hz
	const char* rfport; // Port name
};

static void handle_sig(int sig) {
	printf("Waiting for process to finish...\n");
	stop = true;
}

/* check return value of attr_write function */
static void errchk(int v, const char* what) {
	if (v < 0) { fprintf(stderr, "Error %d writing to channel \"%s\"\nvalue may not be supported.\n", v, what); shutdown_pluto(); }
}

/* write attribute: long long int */
static void wr_ch_lli(struct iio_channel* chn, const char* what, long long val)
{
	errchk(iio_channel_attr_write_longlong(chn, what, val), what);
}

/* write attribute: string */
static void wr_ch_str(struct iio_channel* chn, const char* what, const char* str)
{
	errchk(iio_channel_attr_write(chn, what, str), what);
}

/* helper function generating channel names */
static char* get_ch_name(const char* type, int id)
{
	snprintf(tmpstr, sizeof(tmpstr), "%s%d", type, id);
	return tmpstr;
}

/* returns ad9361 phy device */
static struct iio_device* get_ad9361_phy(struct iio_context* ctx)
{
	dev = iio_context_find_device(ctx, "ad9361-phy");
	assert(dev && "No ad9361-phy found");
	return dev;
}

/* finds AD9361 streaming IIO devices */
static bool get_ad9361_stream_dev(struct iio_context* ctx, enum iodev d, struct iio_device** dev)
{
	switch (d) {
	case TX: *dev = iio_context_find_device(ctx, "cf-ad9361-dds-core-lpc"); return *dev != NULL;
	case RX: *dev = iio_context_find_device(ctx, "cf-ad9361-lpc");  return *dev != NULL;
	default: assert(0); return false;
	}
}

/* finds AD9361 streaming IIO channels */
static bool get_ad9361_stream_ch(struct iio_context* ctx, enum iodev d, struct iio_device* dev, int chid, struct iio_channel** chn)
{
	*chn = iio_device_find_channel(dev, get_ch_name("voltage", chid), d == TX);
	if (!*chn)
		*chn = iio_device_find_channel(dev, get_ch_name("altvoltage", chid), d == TX);
	return *chn != NULL;
}

/* finds AD9361 phy IIO configuration channel with id chid */
static bool get_phy_chan(struct iio_context* ctx, enum iodev d, int chid, struct iio_channel** chn)
{
	switch (d) {
	case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), false); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), true);  return *chn != NULL;
	default: assert(0); return false;
	}
}

/* finds AD9361 local oscillator IIO configuration channels */
static bool get_lo_chan(struct iio_context* ctx, enum iodev d, struct iio_channel** chn)
{
	switch (d) {
		// LO chan is always output, i.e. true
	case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 0), true); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 1), true); return *chn != NULL;
	default: assert(0); return false;
	}
}

/* applies streaming configuration through IIO */
bool cfg_ad9361_streaming_ch(struct iio_context* ctx, struct stream_cfg* cfg, enum iodev type, int chid)
{
	struct iio_channel* chn = NULL;

	// Configure phy and lo channels
	printf("* Acquiring AD9361 phy channel %d\n", chid);
	if (!get_phy_chan(ctx, type, chid, &chn)) { return false; }
	wr_ch_str(chn, "rf_port_select", cfg->rfport);
	wr_ch_lli(chn, "rf_bandwidth", cfg->bw_hz);
	wr_ch_lli(chn, "sampling_frequency", cfg->fs_hz);
	// Configure LO channel
	printf("* Acquiring AD9361 %s lo channel\n", type == TX ? "TX" : "RX");
	if (!get_lo_chan(ctx, type, &chn)) { return false; }
	wr_ch_lli(chn, "frequency", cfg->lo_hz);
	return true;
}


/* simple configuration and streaming */
bool pluto()
{
	// resetting in case of
	if (rx0_i) { iio_channel_disable(rx0_i); }
	if (rx0_q) { iio_channel_disable(rx0_q); }
	if (tx0_i) { iio_channel_disable(tx0_i); }
	if (tx0_q) { iio_channel_disable(tx0_q); }
	
	bool check;


	// Streaming devices

	struct iio_device* aa;

	// Stream configurations
	struct stream_cfg txcfg;

	// Listen to ctrl+c and assert
	signal(SIGINT, handle_sig);

	// TX stream config
	txcfg.bw_hz = 16000000; // 1.5 MHz rf bandwidth
	txcfg.fs_hz = 30720000;   // 2.5 MS/s tx sample rate
	txcfg.lo_hz = GHZ(2.4); // 2.5 GHz rf frequency
	txcfg.rfport = "A"; // port A (select for rf freq.)


	printf("* Acquiring IIO context\n");
	check = (ctx = iio_create_context_from_uri("ip:192.168.2.1"));
	if (check == false) {
		printf("No context");
		return false;
	}

	check = (iio_context_get_devices_count(ctx) > 0);
	if (check == false) {
		printf("No devices");
		return false;
	}

	printf("* Acquiring AD9361 streaming devices\n");
	check = (get_ad9361_stream_dev(ctx, TX, &tx));
	if (check == false) {
		printf("No tx dev found");
		return false;
	}


	printf("* Configuring AD9361 for streaming\n");
	check = (cfg_ad9361_streaming_ch(ctx, &txcfg, TX, 0));
	if (check == false) {
		printf("TX port 0 not found");
		return false;
	}

	printf("* Initializing AD9361 IIO streaming channels\n");
	check = (get_ad9361_stream_ch(ctx, TX, tx, 0, &tx0_i));
	if (check == false) {
		printf("TX chan i not found");
		return false;
	}
	check = (get_ad9361_stream_ch(ctx, TX, tx, 1, &tx0_q));
	if (check == false) {
		printf("TX chan q not found");
		return false;
	}

	printf("* Enabling IIO streaming channels\n");
	iio_channel_enable(tx0_i);
	iio_channel_enable(tx0_q);

	printf("* Creating non-cyclic IIO buffers with 1 MiS\n \n");
	txbuf = iio_device_create_buffer(tx, buffer_size, true);
	if (!txbuf) {
		perror("Could not create TX buffer");
		shutdown_pluto();
		return false;
	}


	float freq_sin = 10000;

	sin_gen(freq_sin);


	return true;
}