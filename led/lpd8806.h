#include <string>

class LPD8806 {
private:

	int _ledCount;
	int _byteCount;
	int _latchBytes;
	int _totalBytes;

	char _gamma[256];
	char* _buffer;

	std::string _dev;
	int _spifd;

	int _errorCode;

public:

	LPD8806() {}
	LPD8806(int ledCount, std::string dev);
	~LPD8806();

	int getErrorCode() const { return _errorCode; }

	void update();

	void fill(char red, char green, char blue);

	void setPixel(int pixel, char red, char green, char blue);
};
