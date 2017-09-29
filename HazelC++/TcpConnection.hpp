#pragma once

#include "Hazel.hpp"
#include "NetworkConnection.hpp"

#include <Poco/Net/SocketStream.h>

namespace Hazel
{
	class TcpConnection : public NetworkConnection
	{
		Poco::Net::StreamSocket *soc;

		std::mutex socket_mutex;

		TcpConnection(Poco::Net::StreamSocket &soc);
		TcpConnection(NetworkEndPoint end_point);
	};
}