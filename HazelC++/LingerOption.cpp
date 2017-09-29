#include "LingerOption.hpp"

namespace Hazel
{
	LingerOption::LingerOption(bool enable, int seconds)
	{
		enabled = enable;
		linger_time = seconds;
	}

	bool LingerOption::IsEnabled()
	{
		return enabled;
	}

	int LingerOption::GetLingerTime()
	{
		return linger_time;
	}
}