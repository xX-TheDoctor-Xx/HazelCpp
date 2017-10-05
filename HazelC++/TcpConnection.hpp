#pragma once

#include "Hazel.hpp"
#include "NetworkConnection.hpp"

namespace Hazel
{
	class TcpConnection : public NetworkConnection
	{
		std::mutex socket_mutex;

		TcpConnection(NetworkEndPoint end_point);
	};
}