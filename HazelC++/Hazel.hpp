#pragma once

#include <mutex>

#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/TCPServer.h>

#include <Poco/Net/DatagramSocket.h>

#include <climits>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

typedef int SOCKET;

void closesocket(SOCKET socket)
{
	close(socket);
}
#endif

typedef unsigned char byte;

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
		Unknown = -1,
		Unspecified = 0,
		Unix = 1,
		InterNetwork = 2,
		ImpLink = 3,
		Pup = 4,
		Chaos = 5,
		NS = 6,
		Ipx = 6,
		Iso = 7,
		Osi = 7,
		Ecma = 8,
		DataKit = 9,
		Ccitt = 10,
		Sna = 11,
		DecNet = 12,
		DataLink = 13,
		Lat = 14,
		HyperChannel = 15,
		AppleTalk = 16,
		NetBios = 17,
		VoiceView = 18,
		FireFox = 19,
		Banyan = 21,
		Atm = 22,
		InterNetworkV6 = 23,
		Cluster = 24,
		Ieee12844 = 25,
		Irda = 26,
		NetworkDesigners = 28,
		Max = 29
	};

	enum class SocketType
	{
		Unknown = -1,
		Stream = 1,
		Dgram = 2,
		Raw = 3,
		Rdm = 4,
		Seqpacket = 5
	};

	enum class ProtocolType
	{
		Unknown = -1,
		IP = 0,
		IPv6HopByHopOptions = 0,
		Unspecified = 0,
		Icmp = 1,
		Igmp = 2,
		Ggp = 3,
		IPv4 = 4,
		Tcp = 6,
		Pup = 12,
		Udp = 17,
		Idp = 22,
		IPv6 = 41,
		IPv6RoutingHeader = 43,
		IPv6FragmentHeader = 44,
		IPSecEncapsulatingSecurityPayload = 50,
		IPSecAuthenticationHeader = 51,
		IcmpV6 = 58,
		IPv6NoNextHeader = 59,
		IPv6DestinationOptions = 60,
		ND = 77,
		Raw = 255,
		Ipx = 1000,
		Spx = 1256,
		SpxII = 1257
	};

	enum class SocketError
	{
		SocketError = -1,
		Success = 0,
		OperationAborted = 995,
		IOPending = 997,
		Interrupted = 10004,
		AccessDenied = 10013,
		Fault = 10014,
		InvalidArgument = 10022,
		TooManyOpenSockets = 10024,
		WouldBlock = 10035,
		InProgress = 10036,
		AlreadyInProgress = 10037,
		NotSocket = 10038,
		DestinationAddressRequired = 10039,
		MessageSize = 10040,
		ProtocolType = 10041,
		ProtocolOption = 10042,
		ProtocolNotSupported = 10043,
		SocketNotSupported = 10044,
		OperationNotSupported = 10045,
		ProtocolFamilyNotSupported = 10046,
		AddressFamilyNotSupported = 10047,
		AddressAlreadyInUse = 10048,
		AddressNotAvailable = 10049,
		NetworkDown = 10050,
		NetworkUnreachable = 10051,
		NetworkReset = 10052,
		ConnectionAborted = 10053,
		ConnectionReset = 10054,
		NoBufferSpaceAvailable = 10055,
		IsConnected = 10056,
		NotConnected = 10057,
		Shutdown = 10058,
		TimedOut = 10060,
		ConnectionRefused = 10061,
		HostDown = 10064,
		HostUnreachable = 10065,
		ProcessLimit = 10067,
		SystemNotReady = 10091,
		VersionNotSupported = 10092,
		NotInitialized = 10093,
		Disconnecting = 10101,
		TypeNotFound = 10109,
		HostNotFound = 11001,
		TryAgain = 11002,
		NoRecovery = 11003,
		NoData = 11004
	};

	enum class SelectMode
	{
		SelectRead,
		SelectWrite,
		SelectError
	};

	enum class SocketOptionLevel
	{
		IP = 0,
		Tcp = 6,
		Udp = 17,
		IPv6 = 41,
		Socket = 65535
	};

	enum class SocketOptionName
	{
		DontLinger = -129,
		ExclusiveAddressUse = -5,
		Debug = 1,
		IPOptions = 1,
		NoDelay = 1,
		NoChecksum = 1,
		AcceptConnection = 2,
		HeaderIncluded = 2,
		BsdUrgent = 2,
		Expedited = 2,
		TypeOfService = 3,
		ReuseAddress = 4,
		IpTimeToLive = 4,
		KeepAlive = 8,
		MulticastInterface = 9,
		MulticastTimeToLive = 10,
		MulticastLoopback = 11,
		AddMembership = 12,
		DropMembership = 13,
		DontFragment = 14,
		AddSourceMembership = 15,
		DontRoute = 16,
		DropSourceMembership = 16,
		BlockSource = 17,
		UnblockSource = 18,
		PacketInformation = 19,
		ChecksumCoverage = 20,
		HopLimit = 21,
		Broadcast = 32,
		UseLoopback = 64,
		Linger = 128,
		OutOfBandInline = 256,
		SendBuffer = 4097,
		ReceiveBuffer = 4098,
		SendLowWater = 4099,
		ReceiveLowWater = 4100,
		SendTimeout = 4101,
		ReceiveTimeout = 4102,
		Error = 4103,
		Type = 4104,
		UpdateAcceptContext = 28683,
		UpdateConnectContext = 28688,
		MaxConnections = INT_MAX
	};

	enum class SocketShutdown
	{
		Receive,
		Send,
		Both
	};

	class Util
	{
	public:
		static bool IsBigEndian();
		static bool SupportsIPv6();
		static void BlockCopy(void *source, unsigned int source_offset, void *destination, unsigned int destination_offset, unsigned int count);
	};

	class Lock
	{
		std::mutex &mutex;

	public:
		Lock(std::mutex &mutex);

		~Lock();

		operator bool() const;
	};

	#define lock(x) if (Lock lock = x){}else
}