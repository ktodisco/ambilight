#include "analyzer.h"

Color blendColors(
	const Color& c1,
	const float w1,
	const Color& c2,
	const float w2,
	const Color& c3,
	const float w3) {
	Color blend;
	blend.r = c1.r * w1 + c2.r * w2 + c3.r * w3;
	blend.g = c1.g * w1 + c2.g * w2 + c3.g * w3;
	blend.b = c1.b * w1 + c2.b * w2 + c3.b * w3;
	return blend;
}

Analyzer::Analyzer(int width, int height) :
	_width(width), _height(height) {

	int total = _width + (2 * height);
	for (int i = 0; i < total; ++i) {
		_results.push_back(Color());
		_previous.push_back(Color());
	}
}

Analyzer::~Analyzer() {
	_results.clear();
}

void Analyzer::analyze(cv::Mat& frame) {
	// Get the frame size, calculate the width and height Mat sizes.
	int frameWidth = frame.cols;
	int frameHeight = frame.rows;

	// Two is added to account for the corners, which do not have LEDs.
	int widthCut = frameWidth / (_width + 2);
	int heightCut = frameHeight / (_height + 2);

	// Start in the lower-left, work up the left side.
	for (int i = 0; i < _height; ++i) {
		cv::Rect cut(0, ((_height - i) * heightCut), heightCut, heightCut);
		cv::Mat slice = frame(cut);
		_results[i] = analyzeCut(slice);
	}

	// Start at the top-left, work across the top.
	for (int i = 0; i < _width; ++i) {
		cv::Rect cut(((i + 1) * widthCut), 0, widthCut, widthCut);
		cv::Mat slice = frame(cut);
		_results[_height + i] = analyzeCut(slice);
	}

	// Start at the top-right, work down the right side.
	for (int i = 0; i < _height; ++i) {
		cv::Rect cut(frameWidth - heightCut, ((i + 1) * heightCut), heightCut, heightCut);
		cv::Mat slice = frame(cut);
		_results[_height + _width + i] = analyzeCut(slice);
	}
}

void Analyzer::smooth() {
	const float edgeBlend = 0.25f;
	const float mainBlend = 1.0f - (edgeBlend * 2);

	std::vector<Color> smoothed(_results);

	// Handle the first edge-case.
	smoothed[0] = blendColors(
		_results[0],
		edgeBlend,
		_results[0],
		mainBlend,
		_results[1],
		edgeBlend);
	for (int i = 1; i < _height - 1; ++i) {
		smoothed[i] = blendColors(
			_results[i - 1],
			edgeBlend,
			_results[i],
			mainBlend,
			_results[i + 1],
			edgeBlend);
	}
	// Handle the second edge-case.
	smoothed[_height - 1] = blendColors(
		_results[_height - 2],
		edgeBlend,
		_results[_height - 1],
		mainBlend,
		_results[_height - 1],
		edgeBlend);

	// Handle top edge.
	smoothed[_height] = blendColors(
		_results[_height],
		edgeBlend,
		_results[_height],
		mainBlend,
		_results[_height + 1],
		edgeBlend);
	for (int i = _height + 1; i < _height + _width - 1; ++i) {
		smoothed[i] = blendColors(
			_results[i - 1],
			edgeBlend,
			_results[i],
			mainBlend,
			_results[i + 1],
			edgeBlend);
	}
	// Handle the other top edge.
	smoothed[_height + _width - 1] = blendColors(
		_results[_height + _width - 2],
		edgeBlend,
		_results[_height + _width - 1],
		mainBlend,
		_results[_height + _width - 1],
		edgeBlend);

	// Handle the second side edge case.
	smoothed[_height + _width] = blendColors(
		_results[_height + _width],
		edgeBlend,
		_results[_height + _width],
		mainBlend,
		_results[_height + _width + 1],
		edgeBlend);
	for (int i = _height + _width + 1; i < 2 * _height + _width - 1; ++i) {
		smoothed[i] = blendColors(
			_results[i - 1],
			edgeBlend,
			_results[i],
			mainBlend,
			_results[i + 1],
			edgeBlend);
	}
	// Handle the final corner.
	smoothed[2 * _height + _width - 1] = blendColors(
		_results[2 * _height + _width - 2],
		edgeBlend,
		_results[2 * _height + _width - 1],
		mainBlend,
		_results[2 * _height + _width - 1],
		edgeBlend);

	// Copy the results.
	_results.swap(smoothed);
}

void Analyzer::blend() {
	for (int i = 0; i < _results.size(); ++i) {
		_results[i] = blendColors(
			_results[i],
			0.25f,
			_results[i],
			0.25f,
			_previous[i],
			0.5f);
	}

	_previous = _results;
}

const Color& Analyzer::getResult(int i) {
	return _results[i];
}

Color Analyzer::analyzeCut(cv::Mat& slice) {
	typedef cv::Vec<uint8_t, 3> V;

	int redValue = 0, greenValue = 0, blueValue = 0;

	cv::MatConstIterator_<V> itr = slice.begin<V>();
	for ( ; itr != slice.end<V>(); ++itr) {
		V pixel = *itr;
		uint8_t blue = cv::saturate_cast<uint8_t>(pixel[0]);
		uint8_t green = cv::saturate_cast<uint8_t>(pixel[1]);
		uint8_t red = cv::saturate_cast<uint8_t>(pixel[2]);

		redValue += red;
		greenValue += green;
		blueValue += blue;

	}

	int pixels = slice.rows * slice.cols;
	redValue /= pixels;
	greenValue /= pixels;
	blueValue /= pixels;

	// Colors from the capture card seem to come in on a 39-251 range.
	// Remap to full range.
	const int kLow = 39;
	const int kHigh = 251;

	redValue = mapRange(redValue, kLow, kHigh, 0, 255);
	greenValue = mapRange(greenValue, kLow, kHigh, 0, 255);
	blueValue = mapRange(blueValue, kLow, kHigh, 0, 255);

	return Color(redValue, greenValue, blueValue);
}

int Analyzer::mapRange(int val, int fromLow, int fromHigh, int toLow, int toHigh) {
	float result = toLow + ((float)(toHigh - toLow) / (float)(fromHigh - fromLow)) * (float)(val - fromLow);
	result = result < 0 ? 0 : result;
	result = result > 255 ? 255 : result;
	return (int)result;
}
