#include "NetworkEndPoint.hpp"

namespace Hazel
{
	NetworkEndPoint::NetworkEndPoint()
	{
	}

	NetworkEndPoint::NetworkEndPoint(const std::string & ip, unsigned short port)
	{
		endpoint = Poco::Net::SocketAddress(ip, port);
	}

	void NetworkEndPoint::SetIP(const std::string & ip)
	{
		endpoint = Poco::Net::SocketAddress(ip, endpoint.port());
	}

	void NetworkEndPoint::SetPort(unsigned int port)
	{
		endpoint = Poco::Net::SocketAddress(endpoint.host(), port);
	}

	std::string NetworkEndPoint::GetEndpoint()
	{
		return endpoint.toString();
	}
}