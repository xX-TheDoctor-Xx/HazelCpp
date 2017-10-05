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
		auto fn = [this]()
		{
			// start the listener
		};

		lock(listener_mutex, fn)

		StartListeningForData();
	}

	void UdpConnectionListener::StartListeningForData()
	{
		// WTF???? C#
	}

	void UdpConnectionListener::RemoveConnectionTo(NetworkEndPoint end_point)
	{
		auto fn = [this]()
		{
			//connections.erase(end_point);			
		};

		lock(connection_mutex, fn)
	}
}