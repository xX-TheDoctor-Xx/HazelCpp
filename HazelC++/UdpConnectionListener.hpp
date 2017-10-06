#pragma once

#include "Hazel.hpp"
#include "NetworkConnectionListener.hpp"
#include "UdpServerConnection.hpp"
#include "UdpSocket.hpp"

#include <map>

namespace Hazel
{
	class UdpConnectionListener;

	void read_callback(UdpConnectionListener *listener, Bytes &bytes);
	void send_data_callback(UdpConnectionListener *listener);

	class UdpConnectionListener : public NetworkConnectionListener, public UdpSocket
	{
		friend void read_callback(UdpConnectionListener *listener, Bytes &bytes);
		friend void send_data_callback(UdpConnectionListener *listener);

		Bytes data_buffer;
		std::map<NetworkEndPoint, UdpServerConnection*> connections;
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