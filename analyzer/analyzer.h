#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

struct Color {
	int r;
	int g;
	int b;

	Color() : r(0), g(0), b(0) {}
	Color(int red, int green, int blue) : r(red), g(green), b(blue) {}
};

Color blendColors(
	const Color& c1,
	const float w1,
	const Color& c2,
	const float w2,
	const Color& c3,
	const float w3);

class Analyzer {
private:

	// Width and height are in number of LEDs in each dimension.
	int _width;
	int _height;

	// Results are calculated starting in the lower-left of the picture,
	// in a clockwise direction.
	std::vector<Color> _results;

	// The previous frame is stored to blend between frames.
	std::vector<Color> _previous;

public:

	Analyzer() {}
	Analyzer(int width, int height);
	~Analyzer();

	void analyze(cv::Mat& frame);
	void smooth();
	void blend();

	const Color& getResult(int i);

private:

	Color analyzeCut(cv::Mat& slice);
};
