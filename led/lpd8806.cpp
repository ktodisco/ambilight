#include "lpd8806.h"

#include <assert.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

LPD8806::LPD8806(int ledCount, std::string dev) :
	_ledCount(ledCount), _dev(dev), _errorCode(0) {

	_byteCount = _ledCount * 3;
	_latchBytes = (_ledCount + 31) / 32;
	_totalBytes = _byteCount + _latchBytes;

	_buffer = (char*)malloc(sizeof(char) * _totalBytes);
	memset(_buffer, 0, sizeof(char) * _totalBytes);

	// Color calculations from
	// http://learn.adafruit.com/light-painting-with-raspberry-pi
	for (int i = 0; i < 256; ++i) {
		_gamma[i] = (char)(0x80 | (int)(pow((float)i / 255.0f, 2.5f) * 127.0f + 0.5f));
	}

	_spifd = open(_dev.c_str(), O_WRONLY);
	if (_spifd < 0) {
		printf("Could not open %s.\n", _dev.c_str());
		_errorCode = 1;
	}
}

LPD8806::~LPD8806() {
	free(_buffer);
}

void LPD8806::update() {
	int ret;

	struct spi_ioc_transfer tr;
	memset(&tr, 0, sizeof(spi_ioc_transfer));
	tr.tx_buf = (unsigned long)_buffer;
	tr.rx_buf = 0;
	tr.len = _totalBytes;
	tr.delay_usecs = 0;
	tr.speed_hz = 500000;
	tr.bits_per_word = 8;

	ret = ioctl(_spifd, SPI_IOC_MESSAGE(1), &tr);

	if (ret < 1) {
		printf("Failed to send SPI data: %d.\n", ret);
		_errorCode = 2;
		exit(1);
	}
}

void LPD8806::fill(char red, char green, char blue) {
	for (int i = 0; i < _ledCount; ++i) {
		setPixel(i, red, green, blue);
	}
}

void LPD8806::setPixel(int pixel, char red, char green, char blue) {
	assert(pixel >= 0);
	assert(pixel < _ledCount);

	_buffer[pixel * 3] = _gamma[green];
	_buffer[pixel * 3 + 1] = _gamma[red];
	_buffer[pixel * 3 + 2] = _gamma[blue];
}
