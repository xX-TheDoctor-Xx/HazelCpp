#pragma once

#undef min
#undef max

#include <mutex>

#ifdef _WIN32

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SOCKET_ERRNO ((long)(::WSAGetLastError()))

#undef GetObject

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

typedef int SOCKET;

void closesocket(SOCKET socket)
{
	close(socket);
}

#define SOCKET_ERROR (-1)
#define SOCKET_ERRNO (errno)

#endif

#define MSG_DONTWAIT 0

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned long ulong;

template<typename ...Args>
std::function<void(Args...)> empty_fn;

namespace Hazel
{
	class HazelInit
	{
		static bool initialized;

	public:
		static bool Initialize();
		static void Cleanup();
	};

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

	enum class IPMode
	{
		IPv4,
		IPv6
	};

	#define lock_mutex(x, fn) { std::lock_guard<std::mutex> lock(x); fn(); }
#define lock_mutex_return(x, fn) { std::lock_guard<std::mutex> lock(x); return fn(); }
}