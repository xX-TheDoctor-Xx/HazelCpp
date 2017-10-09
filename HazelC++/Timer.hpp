#pragma once

#include <functional>
#include <thread>

#include <vector>
#include <mutex>

namespace Hazel
{
	template<typename... Args>
	class Timer
	{
	private:
		bool started;
		std::thread t;
		unsigned long interval;
		bool stop;

		static void ActualTimer(Timer *timer, ...);

	public:
		std::function<void(Args...)> Callback;

		Timer();

		/* Set the interval in milliseconds */
		void SetCallback(std::function<void(Args...)> &callback);
		void SetInterval(unsigned long msInterval);
		void Start(...);
		bool IsRunning();
		void Stop();
		void Reset();
	};
}