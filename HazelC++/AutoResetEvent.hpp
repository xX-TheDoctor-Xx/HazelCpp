#pragma once

#include <mutex>

namespace Hazel
{
	class AutoResetEvent
	{
		std::mutex mutex;
		std::unique_lock<std::mutex> lock;
		std::condition_variable cv;
	public:
		AutoResetEvent();

		void Notify();

		bool Wait(int timeout);
	};
}