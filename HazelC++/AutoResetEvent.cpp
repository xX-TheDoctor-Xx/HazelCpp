#include "AutoResetEvent.hpp"

namespace Hazel
{
	AutoResetEvent::AutoResetEvent() : lock(mutex)
	{
	}

	void AutoResetEvent::Notify()
	{
		cv.notify_one();
	}

	bool AutoResetEvent::Wait(int timeout)
	{
		return cv.wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::no_timeout;
	}
}