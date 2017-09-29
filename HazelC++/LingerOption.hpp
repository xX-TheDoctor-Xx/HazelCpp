#pragma once

namespace Hazel
{
	class LingerOption
	{
		bool enabled;
		int linger_time;

	public:
		LingerOption(bool enable, int seconds);

		bool IsEnabled();
		int GetLingerTime();
	};
}