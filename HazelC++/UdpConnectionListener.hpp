#pragma once

#include "Hazel.hpp"
#include "NetworkConnectionListener.hpp"
#include "UdpServerConnection.hpp"
#include "UdpSocket.hpp"

namespace Hazel
{
	class UdpConnectionListener;

	void read_callback(UdpConnectionListener *listener, Bytes &bytes);

	class UdpConnectionListener : public NetworkConnectionListener, public UdpSocket
	{
		friend void read_callback(UdpConnectionListener *listener, Bytes &bytes);

		Bytes data_buffer;
		std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>> connections;
		std::mutex listener_mutex;
		std::mutex connections_mutex;

		void RemoveConnectionTo(NetworkEndPoint end_point);

		void SendData(Bytes bytes, NetworkEndPoint end_point);

	public:
		UdpConnectionListener(NetworkEndPoint end_point);
		void Start() override;
		void StartListeningForData();
		void Close();
	};
}