#pragma once

#include "Hazel.hpp"
#include "NetworkConnectionListener.hpp"
#include "UdpServerConnection.hpp"
#include "UdpSocket.hpp"

namespace Hazel
{
	class UdpConnectionListener;

	void udp_read_callback_listener(Socket *listener);

	class UdpConnectionListener : public NetworkConnectionListener, public UdpSocket
	{
		friend void udp_read_callback_listener(Socket *listener);

		std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>> connections;
		std::mutex listener_mutex;
		std::mutex connections_mutex;

		IPMode mode;

		void RemoveConnectionTo(NetworkEndPoint end_point);

		void SendData(Bytes bytes, NetworkEndPoint end_point);

	public:
		UdpConnectionListener(NetworkEndPoint end_point, IPMode mode);
		void Start() override;
		void StartListeningForData();
		void Close();
	};
}