#pragma once

#include "UdpConnection.hpp"
#include "NetworkEndPoint.hpp"
#include "Bytes.hpp"
#include "UdpSocket.hpp"

namespace Hazel
{
	class UdpClientConnection;

	void read_callback(UdpClientConnection *con, Bytes &bytes, bool has_error);
	void write_bytes_to_connection_callback(UdpClientConnection *con);
	void hello_func(UdpClientConnection *con);

	class UdpClientConnection : public UdpConnection, protected UdpSocket
	{
		friend void read_callback(UdpClientConnection *con, Bytes &bytes, bool has_error);
		friend void write_bytes_to_connection_callback(UdpClientConnection *con);
		friend void hello_func(UdpClientConnection *con);
		
		Bytes data_buffer;
		IPMode mode;

		void StartListeningForData();

	public:
		UdpClientConnection(NetworkEndPoint &remote_end_point, IPMode mode);
		void Connect(Bytes &bytes = Bytes(nullptr, -1), int timeout = 5000) override;

		void HandleDisconnect(HazelException &e) override;

	protected:
		void WriteBytesToConnection(Bytes &bytes) override;
		void Close();
	};
}