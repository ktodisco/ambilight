#include <opencv2/opencv.hpp>

#include <ctime>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

cv::VideoCapture cap;
cv::Mat raw;
cv::Mat frame;
cv::Mat blend;

pthread_t tid[2];
pthread_mutex_t lock;

bool terminate = false;

void* decode(void* arg) {
	while (!terminate) {
		pthread_mutex_lock(&lock);
		//raw = cv::Mat::zeros(raw.rows, raw.cols, raw.type());
		//cap >> raw;
		if (cap.grab()) {
			raw = cv::Mat::zeros(raw.rows, raw.cols, raw.type());
			cap.retrieve(raw);
		}
		else {
			printf("Could not retrieve frame.\n");
		}
		pthread_mutex_unlock(&lock);
		usleep(1000);
	}

	return 0;
}

void* display(void* arg) {
	while (!terminate) {
		pthread_mutex_lock(&lock);
		cv::imshow("video", frame);
		pthread_mutex_unlock(&lock);
		usleep(1000);
	}

	return 0;
}

int main() {
	cap = cv::VideoCapture(0);
	cap >> raw;
	raw.copyTo(blend);
	cv::Rect clip(10, 10, 690, 460);
	blend = blend(clip);

	pthread_create(&(tid[0]), NULL, &decode, NULL);
	usleep(1000 * 1000);

	cv::namedWindow("video", cv::WINDOW_AUTOSIZE);

	//pthread_create(&(tid[1]), NULL, &display, NULL);

	while (!terminate) {
		pthread_mutex_lock(&lock);
		frame = raw(clip);
		cv::addWeighted(blend, 0.8, frame, 0.2, 0.0, blend);
		pthread_mutex_unlock(&lock);

		cv::imshow("video", blend);

		if (cv::waitKey(1) >= 0) {
			terminate = true;
		}
	}

	pthread_join(tid[0], NULL);
	//pthread_join(tid[1], NULL);

	cap.release();
	cv::destroyAllWindows();

	return 0;
}
