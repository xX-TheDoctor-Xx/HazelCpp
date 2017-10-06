#pragma once

#include "Hazel.hpp"
#include "UdpServerConnection.hpp"

#include <map>

namespace Hazel
{
	class Util
	{
	public:
		static bool IsBigEndian();
		static bool SupportsIPv6();
		static void BlockCopy(void *source, unsigned int source_offset, void *destination, unsigned int destination_offset, unsigned int count);
		static bool FillAddr(const std::string & localAddress, unsigned short localPort, sockaddr_in *localAddr);
		static bool FillAddr(const std::string & localAddress, unsigned short localPort, sockaddr *localAddr);

		static int LookForEndPoint(std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>> &map, NetworkEndPoint &end_point);
	};
}