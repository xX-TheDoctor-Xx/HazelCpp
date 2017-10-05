#include "TcpServer.hpp"

namespace Hazel
{
	bool TcpServer::connect(const NetworkEndPoint & ip_addr)
	{
		(void)ip_addr;
		return false;
	}

	bool TcpServer::connect()
	{
		return false;
	}
}