#pragma once
// =================================
// Created by Lipson on 6/1/17.
// Email to LipsonChan@yahoo.com
// Copyright (c) 2017 Lipson. All rights reserved.
// Version 1.0
// =================================

#include <chrono>

using namespace std;
using namespace std::chrono;

class Timer
{
public:
	Timer() : m_begin(high_resolution_clock::now()) {}
	void reset() { m_begin = high_resolution_clock::now(); }

	int64_t elapsed() const
	{
		return duration_cast<chrono::milliseconds>(high_resolution_clock::now() - m_begin).count();
	}

	double elapsed_second() const
	{
		return duration_cast<duration<double>>(high_resolution_clock::now() - m_begin).count();
	}

	int64_t elapsed_micro() const
	{
		return duration_cast<chrono::microseconds>(high_resolution_clock::now() - m_begin).count();
	}

	int64_t elapsed_nano() const
	{
		return duration_cast<chrono::nanoseconds>(high_resolution_clock::now() - m_begin).count();
	}

	int64_t elapsed_minutes() const
	{
		return duration_cast<chrono::minutes>(high_resolution_clock::now() - m_begin).count();
	}

	int64_t elapsed_hours() const
	{
		return duration_cast<chrono::hours>(high_resolution_clock::now() - m_begin).count();
	}

private:
	time_point<high_resolution_clock> m_begin;
};