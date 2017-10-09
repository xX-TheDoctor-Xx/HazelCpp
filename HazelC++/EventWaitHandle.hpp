#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

namespace Hazel
{
	class EventWaitHandle 
	{
	public:
		EventWaitHandle(bool auto_reset, bool initial_value);

		void set();
		void reset();
		bool wait(int ms);

	private:
		bool auto_reset;

		std::condition_variable cv;
		std::atomic<bool> check;
		std::mutex mutex;
	};
}