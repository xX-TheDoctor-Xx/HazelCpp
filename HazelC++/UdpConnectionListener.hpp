#pragma once

#include "Hazel.hpp"
#include "NetworkConnectionListener.hpp"
#include "UdpServerConnection.hpp"

#include <map>

namespace Hazel
{
	class UdpConnectionListener : public NetworkConnectionListener
	{
		Bytes buffer;
		//std::map<NetworkEndPoint, UdpServerConnection> connections;
		std::mutex listener_mutex;
		std::mutex connection_mutex;

		void RemoveConnectionTo(NetworkEndPoint end_point);

	public:
		UdpConnectionListener(NetworkEndPoint end_point);
		void Start() override;
		void StartListeningForData();
	};
}