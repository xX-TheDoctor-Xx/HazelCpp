#pragma once

#include "UdpConnection.hpp"
#include "NetworkEndPoint.hpp"
#include "Bytes.hpp"
#include "UdpSocket.hpp"

namespace Hazel
{
	class UdpClientConnection;

	void read_callback(UdpClientConnection *con, Bytes &bytes, bool has_error);

	class UdpClientConnection : public UdpConnection, public UdpSocket
	{
		friend void read_callback(UdpClientConnection *con, Bytes &bytes, bool has_error);

	public:
		std::mutex socket_mutex;

	private:
		Bytes data_buffer;

		void StartListeningForData();

	public:
		UdpClientConnection(NetworkEndPoint remote_end_point);
		void Connect(Bytes bytes = Bytes(nullptr, -1), int timeout = 5000) override;

		void HandleDisconnect(HazelException &e) override;

	protected:
		void WriteBytesToConnection(Bytes bytes) override;
		void Close();
	};
}