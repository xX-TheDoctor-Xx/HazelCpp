#include "EventWaitHandle.hpp"

namespace Hazel
{
	EventWaitHandle::EventWaitHandle(bool auto_reset, bool initial_value) : auto_reset(auto_reset), check(initial_value)
	{
	}

	void EventWaitHandle::set()
	{
		{
			std::lock_guard<std::mutex> check_lock(mutex);
			check = true;
		}

		cv.notify_one();
	}

	void EventWaitHandle::reset()
	{
		{
			std::lock_guard<std::mutex> check_lock(mutex);
			check = false;
		}

		cv.notify_one();
	}

	bool EventWaitHandle::wait(int ms)
	{
		{
			std::lock_guard<std::mutex> check_lock(mutex);

			if (check)
			{
				check = !auto_reset;
				return true;
			}
			if (ms < 1)
				return false;
		}

		auto start = std::chrono::system_clock::now();
		std::unique_lock<std::mutex> cv_lock(mutex);

		return cv.wait_until(cv_lock, start + std::chrono::milliseconds(ms)) == std::cv_status::no_timeout;
	}
}