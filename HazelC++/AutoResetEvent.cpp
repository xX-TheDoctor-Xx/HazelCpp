#include "AutoResetEvent.hpp"

namespace Hazel
{
	AutoResetEvent::AutoResetEvent(bool initial_value) : EventWaitHandle(true, initial_value)
	{
	}
}