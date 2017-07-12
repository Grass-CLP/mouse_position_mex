#include "mouse_position.h"
#include "timer.h"

MousePositionCapture * MousePositionCapture::one = nullptr;

MousePositionCapture& MousePositionCapture::instance() {
	if (!MousePositionCapture::one) {
		one = new MousePositionCapture();
	}

	return *one;
}

MousePositionCapture::MousePositionCapture(int size, int hz) {
	init(size, hz);
}

MousePositionCapture::~MousePositionCapture() {
	stop();

	if (this->data) {
		unique_lock<mutex> lock(mut);
		free(this->data);
		this->data = nullptr;
	}
}

void MousePositionCapture::init(int size, int hz) {
	this->size = size;
	this->data = (INT64 *)malloc(size * sizeof(INT64) * COLUMN_NUM);
	this->waitIntervalUs = 1000000 / hz;
}

void MousePositionCapture::start() {
	if (!isRunning) {
		isRunning = true;
		this->thd = thread(&MousePositionCapture::capture, this);
		this_thread::sleep_for(std::chrono::milliseconds(1));

		reset();
	}
}

void MousePositionCapture::stop() {
	if (isRunning) {
		isRunning = false;
		this->thd.join();

		reset();
	}
}

void MousePositionCapture::setHiFi(bool hifi) {
	this->hifi = hifi;
}

void MousePositionCapture::setFrequency(int hz) {
	if (hz <= CAPTURE_HZ) {
		this->waitIntervalUs = 1000000 / hz;
	}
}

bool MousePositionCapture::toggleHiFi() {
	this->hifi = !hifi;
	return this->hifi;
}

int MousePositionCapture::captureImage() {
	unique_lock<mutex> lock(mut);
	endTime = GetTime();
	return this->index;
}

void MousePositionCapture::load(INT64* data, int size) {
	INT64 interval = size <= 1 ? 0 : (endTime - beginTime) / (size - 1);

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < COLUMN_NUM; ++j) {
			data[size * j + i] = this->data[COLUMN_NUM * i + j];
		}
	}
}

void MousePositionCapture::reset() {
	unique_lock<mutex> lock(mut);

	index = 0;
	beginTime = GetTime();
}

void MousePositionCapture::capture() {
	while (isRunning) {

		if (index == 0) {
			timer.reset();
			doCapture();
		} else {
			INT64 elapsedUs = timer.elapsed_micro();
			bool flag = elapsedUs >= waitIntervalUs || fabs(elapsedUs - waitIntervalUs) < 5;
			
			if (flag) {
				timer.reset();
				doCapture();
			}

			if (!hifi)
				this_thread::sleep_for(std::chrono::microseconds(5));
		}		
	}
}

void MousePositionCapture::doCapture() {
	unique_lock<mutex> lock(mut);
	GetCursorPos(&pt);
	data[index * COLUMN_NUM] = pt.x;
	data[index * COLUMN_NUM + 1] = pt.y;
	data[index * COLUMN_NUM + 2] = GetTime();
	index = (index + 1) % size;
}
