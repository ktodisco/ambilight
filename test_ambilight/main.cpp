#include <opencv2/opencv.hpp>

#include "analyzer.h"
#include <ctime>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define BLEND_FRAMES 4
#define SMOOTH_PASSES 4

cv::VideoCapture cap;
cv::Mat raw;
cv::Mat frame;
cv::Mat blend;

pthread_t tid[1];
pthread_mutex_t lock;

bool terminate = false;

void* decode(void* arg)
{
	while (true) {
		pthread_mutex_lock(&lock);
		if (cap.grab()) {
			raw = cv::Mat::zeros(raw.rows, raw.cols, raw.type());
			cap.retrieve(raw);
		}
		pthread_mutex_unlock(&lock);
		usleep(1000);

		if (terminate) break;
	}

	return 0;
}

int main() {
	std::cout << "Initializing video capture with OpenCV 3.1.0." << std::endl;

	cap = cv::VideoCapture(0);
	cap >> raw;
	raw.copyTo(blend);

	cv::Mat led;
	raw.copyTo(led);

	cv::Rect clip(10, 10, 690, 460);
	blend = blend(clip);

	int width = 28;
	int height = 14;
	Analyzer analyzer(width, height);

	pthread_create(&(tid[0]), NULL, &decode, NULL);

	usleep(1000 * 1000);

	double blendWeight = 1.0 / BLEND_FRAMES;

	cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("led", cv::WINDOW_AUTOSIZE);
	while (true) {
		led.setTo(cv::Scalar(0,0,0));
		//usleep(1000 * 1000);

		//clock_t start = clock();
		//cap.grab();
		//cap.retrieve(raw);

		pthread_mutex_lock(&lock);
		clock_t start = clock();
		//cv::Mat blend(raw[0]);
		//for (int i = 1; i < BUFFER_SIZE; ++i) {
		//	cv::addWeighted(blend, 0.7, raw[i], 0.3, 0.0, blend);
		//}
		//raw = cv::Mat::zeros(raw.rows, raw.cols, raw.type());
		//if (!cap.retrieve(raw)) {
		//	printf("Could not retrieve frame.\n");
		//}
		//cv::addWeighted(blend, 0.4, raw, 0.6, 0.0, blend);
		//cv::medianBlur(raw, raw, 3);
		clock_t end = clock();
		//std::cout << "Blur took " << dur << "ms." << std::endl;

		//std::cout << raw.cols << " " << raw.rows << std::endl;
		frame = raw(clip);
		cv::addWeighted(blend, 1.0 - blendWeight, frame, blendWeight, 0.0, blend);

		pthread_mutex_unlock(&lock);
		//clock_t end = clock();
		//double dur = 1000.0 * (end - start) / CLOCKS_PER_SEC;
		//std::cout << "Frame decode took " << dur << "ms." << std::endl;

		analyzer.analyze(blend);
		start = clock();
		for (int i = 0; i < SMOOTH_PASSES; ++i) {
			analyzer.smooth();
		}
		analyzer.blend();
		end = clock();
		double dur = 1000.0 * (end - start) / CLOCKS_PER_SEC;
		//printf("Smooth took %fms\n", dur);

		//cv::Mat led(frame.rows, frame.cols, CV_8UC3, cv::Scalar(0,0,0));
		int frameWidth = frame.cols;
		int frameHeight = frame.rows;

		int widthCut = frameWidth / (width + 2);
		int heightCut = frameHeight / (height + 2);
		int widthCut2 = widthCut * 2;
		int heightCut2 = heightCut * 2;

		for (int i = 0; i < height; ++i) {
			cv::Rect cut(0, ((height - i) * heightCut), heightCut, heightCut);
			cv::Mat slice = led(cut);
			Color result = analyzer.getResult(i);
			slice.setTo(cv::Scalar(result.b, result.g, result.r));
			//cv::Mat frameCut = frame(cut);
			//frameCut.copyTo(slice);
		}

		for (int i = 0; i < width; ++i) {
			cv::Rect cut(((i + 1) * widthCut), 0, widthCut, widthCut);
			cv::Mat slice = led(cut);
			Color result = analyzer.getResult(height + i);
			slice.setTo(cv::Scalar(result.b, result.g, result.r));
			//cv::Mat frameCut = frame(cut);
			//frameCut.copyTo(slice);
		}

		for (int i = 0; i < height; ++i) {
			cv::Rect cut(frameWidth - heightCut, ((i + 1) * heightCut), heightCut, heightCut);
			cv::Mat slice = led(cut);
			Color result = analyzer.getResult(height + width + i);
			slice.setTo(cv::Scalar(result.b, result.g, result.r));
			//cv::Mat frameCut = frame(cut);
			//frameCut.copyTo(slice);
		}

		cv::imshow("frame", blend);
		cv::imshow("led", led);

		if (cv::waitKey(1) >= 0) {
			terminate = true;
			break;
		}
	}

	pthread_join(tid[0], NULL);

	cap.release();
	cv::destroyAllWindows();

	return 0;
}
