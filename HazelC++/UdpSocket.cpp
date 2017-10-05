#include "UdpSocket.hpp"

namespace Hazel
{
	UdpSocket::UdpSocket() : Socket()
	{
	}

	UdpSocket::UdpSocket(const NetworkEndPoint & adr) : Socket(adr)
	{
	}

	UdpSocket::UdpSocket(const char * adr) : Socket(adr)
	{
	}

	UdpSocket::~UdpSocket()
	{
		Socket::close();
	}

	bool UdpSocket::closed() const
	{
		return Socket::closed();
	}

	long UdpSocket::error() const
	{
		return Socket::error();
	}

	const std::string & UdpSocket::error_text() const
	{
		return Socket::error_text();
	}

	bool UdpSocket::wait(bool * is_recv, bool * is_sent, int timeout_ms)
	{
		return Socket::wait(is_recv, is_sent, timeout_ms);
	}

	bool UdpSocket::create()
	{
		if (!Socket::closed()) {
			Socket::error_text("basic_udp_socket::create(): Already created", true);
			return false;
		}
		else if (!Socket::create(SocketType::Udp)) {
			Socket::error_text("basic_udp_socket::create(): failed to create socket");
			return false;
		}
		return true;
	}

	bool UdpSocket::bind(const char * ip)
	{
		return !ip ? false : UdpSocket::bind(NetworkEndPoint(ip));
	}

	bool UdpSocket::bind(const NetworkEndPoint & ip)
	{
		Socket::ip(ip);
		if (!Socket::closed()) {
			Socket::error_text("basic_udp_socket::bind(): Socket already open", true);
			return false;
		}
		else if (!ip.valid()) {
			Socket::error_text("basic_udp_socket::bind(): IP address not valid", true);
		}
		else if (!ip.port()) {
			Socket::error_text("basic_udp_socket::bind(): No port specified", true);
			return false;
		}
		else if (!Socket::create(SocketType::Tcp)) {
			Socket::error_text("basic_udp_socket::bind(): Could not create UDP socket");
			return false;
		}
		else if (!Socket::bind()) {
			Socket::error_text("basic_udp_socket::bind(): Could not bind socket");
			return false;
		}
		return true;
	}

	void UdpSocket::close()
	{
		Socket::close();
	}

	long UdpSocket::recvfrom(void * data, size_t datasize, NetworkEndPoint & ip)
	{
		return Socket::recvfrom(data, datasize, ip);
	}

	long UdpSocket::sendto(const void * data, size_t datasize, const NetworkEndPoint & ip)
	{
		return Socket::sendto(data, datasize, ip);
	}
}