#pragma once

#include "UdpConnection.hpp"
#include "NetworkEndPoint.hpp"
#include "Bytes.hpp"
#include "UdpSocket.hpp"

namespace Hazel
{
	class UdpClientConnection;

	void udp_read_callback_client(Socket *con);
	void udp_write_bytes_to_connection_callback(Socket *con);
	void udp_hello_func(UdpClientConnection *con);

	class UdpClientConnection : public UdpConnection, public UdpSocket
	{
		friend void udp_read_callback_client(Socket *con);
		friend void udp_write_bytes_to_connection_callback(Socket *con);
		friend void udp_hello_func(UdpClientConnection *con);

	public:
		UdpClientConnection(NetworkEndPoint &remote_end_point);
		void Connect(Bytes &bytes = Bytes(nullptr, -1), int timeout = 5000) override;

		void HandleDisconnect(HazelException &e) override;

		void StartListeningForData();

	protected:
		void WriteBytesToConnection(Bytes &bytes) override;
		void Close();
	};
}