#include "ManualResetEvent.hpp"

namespace Hazel
{
	ManualResetEvent::ManualResetEvent(bool initial_value) : EventWaitHandle(false, initial_value)
	{
	}
}