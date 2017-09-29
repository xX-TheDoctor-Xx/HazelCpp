#include "NetworkConnectionListener.hpp"

namespace Hazel
{
	NetworkEndPoint NetworkConnectionListener::GetEndPoint()
	{
		return end_point;
	}

	void NetworkConnectionListener::SetEndPoint(NetworkEndPoint end_point)
	{
		this->end_point = end_point;
	}
}