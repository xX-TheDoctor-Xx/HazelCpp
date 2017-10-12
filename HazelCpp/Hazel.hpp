#pragma once

#include "Defines.hpp"

#include <functional>

#define MSG_DONTWAIT 0

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned long ulong;

template<typename ...Args>
std::function<void(Args...)> empty_fn;

namespace Hazel
{
	enum class SendOption
	{
		None,
		Reliable,
		FragmentedReliable
	};

	enum class ConnectionState : byte
	{
		NotConnected,
		Connecting,
		Connected,
		Disconnecting
	};

	enum class UdpSendOption : byte
	{
		Hello = 8,
		Disconnect = 9,
		Acknowledgement = 10, 
		Fragment = 11
	};

	enum class AddressFamily 
	{ 
		Any = AF_UNSPEC, 
		IPv4 = AF_INET, 
		IPv6 = AF_INET6 
	};

	enum class SocketType 
	{ 
		Any = 0, 
		Tcp = SOCK_STREAM, 
		Udp = SOCK_DGRAM 
	};
}