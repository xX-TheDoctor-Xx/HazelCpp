#include "NetworkConnectionListener.hpp"

namespace Hazel
{
	NetworkConnectionListener::NetworkConnectionListener()
	{
	}

	NetworkEndPoint NetworkConnectionListener::GetEndPoint()
	{
		return end_point;
	}

	void NetworkConnectionListener::SetEndPoint(NetworkEndPoint end_point)
	{
		this->end_point = end_point;
	}
}