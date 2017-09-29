#include "Stopwatch.hpp"

namespace Hazel
{
	void Stopwatch::Start()
	{
		start_point = std::chrono::high_resolution_clock::now();
	}

	void Stopwatch::Stop()
	{
		stop_point = std::chrono::high_resolution_clock::now();
	}

	long long Stopwatch::GetElapsedMilliseconds()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(stop_point - start_point).count();
	}
}