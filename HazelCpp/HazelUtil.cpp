#include "HazelUtil.hpp"

namespace Hazel
{
	int HazelUtil::LookForEndPoint(std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>> &map, NetworkEndPoint & end_point)
	{
		for (unsigned int i = 0; i < map.size(); i++)
		{
			std::pair<NetworkEndPoint, UdpServerConnection*> pair = map.at(i);

			if (pair.first == end_point)
				return i;
		}

		return -1;
	}
}