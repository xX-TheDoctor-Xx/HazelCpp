#include "Timer.hpp"

namespace Hazel
{
	template<typename... Args>
	Timer<Args...>::Timer()
	{
		callback = 0;
		interval = 1000; /* Set default interval to 1 second */
		started = false;
	}

	template<typename ...Args>
	void Timer<Args...>::SetCallback(GenericFunction<void, Args...> &callback)
	{
		this->callback = callback;
	}

	template<typename... Args>
	void Timer<Args...>::SetInterval(unsigned long msInterval)
	{
		interval = msInterval;
	}

	template<typename... Args>
	void Timer<Args...>::Start()
	{
		if (!started)
			t = std::thread(ActualTimer, this);
		started = true;
	}

	template<typename... Args>
	bool Timer<Args...>::IsRunning()
	{
		return started && !stop;
	}

	template<typename... Args>
	void Timer<Args...>::Stop()
	{
		if (started)
			stop = true;
		started = false;
	}

	template<typename... Args>
	void Timer<Args...>::ActualTimer(Timer * timer)
	{
		while (!timer->stop)
		{
			timer->callback.Call(params[0]); // fix this to include all params
			std::this_thread::sleep_for(std::chrono::milliseconds(timer->interval));
		}


	}
}