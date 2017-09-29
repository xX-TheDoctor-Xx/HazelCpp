#pragma once

#include "UdpConnection.hpp"
#include "NetworkEndPoint.hpp"
#include "Bytes.hpp"

namespace Hazel
{
	class UdpClientConnection : public UdpConnection
	{
		SOCKET soc_ptr;
		std::mutex socket_mutex;

		Bytes data_buffer;

		void StartListeningForData();

	public:
		UdpClientConnection(NetworkEndPoint remote_end_point);
		void Connect(Bytes bytes = Bytes(nullptr, -1), int timeout = 5000) override;

	protected:
		void WriteBytesToConnection(Bytes bytes) override;
	};
}