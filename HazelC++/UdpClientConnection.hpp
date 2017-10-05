#pragma once

#include "UdpConnection.hpp"
#include "NetworkEndPoint.hpp"
#include "Bytes.hpp"
#include "UdpSocket.hpp"

namespace Hazel
{
	class UdpClientConnection : public UdpConnection, public UdpSocket
	{
	public:
		std::mutex socket_mutex;

	private:
		Bytes data_buffer;

		void StartListeningForData();

	public:
		UdpClientConnection(NetworkEndPoint remote_end_point);
		void Connect(Bytes bytes = Bytes(nullptr, -1), int timeout = 5000) override;

	protected:
		void WriteBytesToConnection(Bytes bytes) override;
		void HandleDisconnect(HazelException &e) override;
		void Close();
	};
}