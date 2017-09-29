#pragma once

#include <chrono>

namespace Hazel
{
	class Stopwatch
	{
		std::chrono::time_point<std::chrono::steady_clock> start_point;
		std::chrono::time_point<std::chrono::steady_clock> stop_point;

	public:
		void Start();
		void Stop();
		long long GetElapsedMilliseconds();
	};
}