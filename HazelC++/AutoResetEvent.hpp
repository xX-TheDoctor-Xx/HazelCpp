#pragma once

#include "EventWaitHandle.hpp"

namespace Hazel
{
	class AutoResetEvent : public EventWaitHandle
	{
	public:
		AutoResetEvent(bool initial_value);
	};
}