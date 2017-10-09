#pragma once

#include "EventWaitHandle.hpp"

namespace Hazel
{
	class ManualResetEvent : public EventWaitHandle
	{
	public:
		ManualResetEvent(bool initial_value);
	};
}