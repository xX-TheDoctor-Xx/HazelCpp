#pragma once

#include "ConnectionEndPoint.hpp"
#include "Hazel.hpp"

#include <string>

namespace Hazel
{
	class NetworkEndPoint : public ConnectionEndPoint
	{
		Poco::Net::SocketAddress endpoint;

	public:
		NetworkEndPoint();
		NetworkEndPoint(const std::string &ip, ushort port);

		void SetIP(const std::string &ip);
		void SetPort(unsigned int port);
		std::string GetEndpoint();

		
	};
}