#include "ManualResetEvent.hpp"

namespace Hazel
{
	ManualResetEvent::ManualResetEvent()
	{
	}

	void ManualResetEvent::Set()
	{
		std::unique_lock<std::mutex> lock(mutex);
		signaled = true;
		cv.notify_all();
	}

	void ManualResetEvent::Reset()
	{
		std::unique_lock<std::mutex> lock(mutex);
		signaled = false;
	}
	
	bool ManualResetEvent::Wait(int timeout)
	{
		std::unique_lock<std::mutex> lock(mutex);

		stopwatch.Start();
		while (!signaled && stopwatch.GetElapsedMilliseconds() <= timeout)
		{
			cv.wait_for(lock, std::chrono::milliseconds(timeout));
			stopwatch.Stop(); // resets end_point each time it executes
		}
	}
}