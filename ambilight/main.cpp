#include <opencv2/opencv.hpp>

#include "analyzer.h"
#include "lpd8806.h"

#include <ctime>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#define LED_COUNT 56
#define BLEND_FRAMES 8
#define SMOOTH_PASSES 1

enum OP_MODE {
	NORMAL,
	NO_VIDEO,
};

cv::VideoCapture cap;
cv::Mat raw;
cv::Mat frame;
cv::Mat blend;

pthread_t tid[1];
pthread_mutex_t lock;

bool terminate = false;

void* decode(void* arg) {
	while (!terminate) {
		pthread_mutex_lock(&lock);
		if (cap.grab()) {
			raw = cv::Mat::zeros(raw.rows, raw.cols, raw.type());
			cap.retrieve(raw);
		}
		pthread_mutex_unlock(&lock);
		usleep(1000);
	}
	return 0;
}

void updateVideo(
	cv::VideoCapture& cap,
	Analyzer& analyzer,
	LPD8806& strip) {

	cv::Rect clip(10, 10, 690, 460);

	pthread_mutex_lock(&lock);

	// TODO: This could be made better by waiting for a frame to ready
	// by the decode thread.
	frame = raw(clip);
	double blendFactor = 1.0 / BLEND_FRAMES;
	cv::addWeighted(blend, 1.0 - blendFactor, frame, blendFactor, 0.0, blend);

	pthread_mutex_unlock(&lock);

	analyzer.analyze(blend);
	for (int i = 0; i < SMOOTH_PASSES; ++i) {
		analyzer.smooth();
	}
	analyzer.blend();

	for (int i = 0; i < LED_COUNT; ++i) {
		Color result = analyzer.getResult(i);
		strip.setPixel(i, result.r, result.g, result.b);
	}
	strip.update();
}

void updateNoVideo(LPD8806& strip) {
	strip.fill(255, 0, 0);
	strip.update();
}

int main() {
	// HACK: Sleep for 10 seconds to allow for the Pi to complete its boot
	// process.  Without this, something about the input video stream
	// isn't ready and the values we'll put out to the LEDs will be bad.
	sleep(10);

	std::cout << "Initializing video capture with OpenCV 3.1.0." << std::endl;

	OP_MODE mode = NORMAL;

	cap = cv::VideoCapture(0);
	if (!cap.isOpened()) {
		mode = NO_VIDEO;
	}

	int width = 28;
	int height = 14;
	Analyzer analyzer(width, height);

	LPD8806 strip(LED_COUNT, "/dev/spidev0.0");

	pthread_create(&(tid[0]), NULL, &decode, NULL);

	usleep(1000 * 1000);
	raw.copyTo(blend);
	blend = blend(cv::Rect(10, 10, 690, 460));

	while (true) {
		switch (mode) {
		case NORMAL:
			updateVideo(cap, analyzer, strip);
			break;
		case NO_VIDEO:
			updateNoVideo(strip);
			break;
		}
	}

	terminate = true;
	pthread_join(tid[0], NULL);

	cap.release();

	return 0;
}
