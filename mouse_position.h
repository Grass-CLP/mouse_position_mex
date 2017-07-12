#pragma once

#include <windows.h>
#include <atomic>
#include <thread>
#include <mutex>
#include "timer.h"

static const unsigned __int64 epoch = ((unsigned __int64)116444736000000000ULL);
static int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	FILETIME    file_time;
	SYSTEMTIME  system_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);

	return 0;
}

static INT64 GetTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return static_cast<INT64>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

constexpr int CAPTURE_HZ = 300;
constexpr int COLUMN_NUM = 3;
constexpr int MOUSE_DATA_SIZE = CAPTURE_HZ * 60 * 60;	// 1 houer 300Hz
using namespace std;

class MousePositionCapture {

public:
	static MousePositionCapture *one;
	static MousePositionCapture& instance();

	MousePositionCapture(int size = MOUSE_DATA_SIZE, int hz = CAPTURE_HZ);
	~MousePositionCapture();

	void init(int size = MOUSE_DATA_SIZE, int hz = CAPTURE_HZ);
	void start();
	void stop();
	void setHiFi(bool hifi = true);
	void setFrequency(int hz);
	bool toggleHiFi();
	int captureImage();
	void load(INT64 *data, int size);
	void reset();

private:
	void capture();
	void doCapture();

private:
	std::mutex mut;
	std::thread thd;
	int index = 0;
	INT64 beginTime = 0, endTime = 0;
	atomic<bool> isRunning = false;
	POINT pt;
	INT64 *data = nullptr;
	int size = 0;
	INT64 waitIntervalUs = 0;
	Timer timer;
	bool hifi = true;
};