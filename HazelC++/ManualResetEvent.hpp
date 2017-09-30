#pragma once

#include <mutex>
#include "Stopwatch.hpp"

namespace Hazel
{
	class ManualResetEvent
	{
		std::mutex mutex;
		std::condition_variable cv;
		Stopwatch stopwatch;

		bool signaled;
	public:
		ManualResetEvent();

		void Set();
		void Reset();

		bool Wait(int timeout); // milliseconds
	};
}