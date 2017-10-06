#include "Util.hpp"

namespace Hazel
{
	bool Util::IsBigEndian()
	{
		return (htonl(47) == 47 ? true : false);
	}

	bool Util::SupportsIPv6()
	{
		return true;
	}


	void Util::BlockCopy(void * source, unsigned int source_offset, void * destination, unsigned int destination_offset, unsigned int count)
	{
		char *src = (char*)source;
		char *dst = (char*)destination;
		memcpy(dst + destination_offset, src + source_offset, count);
	}

	bool Util::FillAddr(const std::string & address, unsigned short port, sockaddr_in *addr)
	{
		std::memset(&addr, 0, sizeof(addr));
		addr->sin_family = AF_INET; // can be ipv6 right?

		hostent *host;
		if (!(host = gethostbyname(address.c_str())))
			return false;
		addr->sin_addr.s_addr = inet_addr(host->h_addr_list[0]);
		addr->sin_port = htons(port);

		return true;
	}

	bool Util::FillAddr(const std::string & address, unsigned short port, sockaddr *addr)
	{
		return Util::FillAddr(address, port, (sockaddr_in*)addr);
	}

	int Util::LookForEndPoint(std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>> &map, NetworkEndPoint & end_point)
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