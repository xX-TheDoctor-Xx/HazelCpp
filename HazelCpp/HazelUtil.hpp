#pragma once

#include "Hazel.hpp"
#include "UdpServerConnection.hpp"

#include <map>

namespace Hazel
{
	class HazelUtil
	{
	public:
		static int LookForEndPoint(std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>> &map, NetworkEndPoint &end_point);
	};
}