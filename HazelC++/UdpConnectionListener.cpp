#include "UdpConnectionListener.hpp"

namespace Hazel
{
	UdpConnectionListener::UdpConnectionListener(NetworkEndPoint end_point) : buffer(new byte[65535](), 65535)
	{
		SetEndPoint(end_point);

		//create listener
	}

	void UdpConnectionListener::Start()
	{
		lock(listener_mutex)
		{
			// start the listener
		}

		StartListeningForData();
	}

	void UdpConnectionListener::StartListeningForData()
	{
		// WTF???? C#
	}

	void UdpConnectionListener::RemoveConnectionTo(NetworkEndPoint end_point)
	{
		lock(connection_mutex)
		{
			//connections.erase(end_point);
		}
	}
}