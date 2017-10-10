#include "TcpSocket.hpp"

namespace Hazel
{
	TcpSocket::TcpSocket() : Socket()
	{
	}

	TcpSocket::TcpSocket(SOCKET id) : Socket(id)
	{
	}

	TcpSocket::TcpSocket(const NetworkEndPoint & adr) : Socket(adr)
	{
	}

	TcpSocket::TcpSocket(SOCKET id, const NetworkEndPoint & adr) : Socket(id, adr)
	{
	}

	TcpSocket::TcpSocket(const char * adr) : Socket(adr)
	{
	}

	TcpSocket::~TcpSocket()
	{
	}

	const NetworkEndPoint & TcpSocket::ip() const
	{
		return Socket::ip();
	}

	void TcpSocket::ip(const NetworkEndPoint & adr)
	{
		Socket::ip(adr);
	}

	void TcpSocket::ip(const char * adr)
	{
		Socket::ip(adr);
	}

	long TcpSocket::error() const throw()
	{
		return Socket::error();
	}

	const std::string & TcpSocket::error_text() const
	{
		return Socket::error_text();
	}

	void TcpSocket::error(long newerror)
	{
		return Socket::error(newerror);
	}

	void TcpSocket::error_text(const char * text)
	{
		return Socket::error_text(text);
	}

	bool TcpSocket::connect(const char * iptext)
	{
		return connect(NetworkEndPoint(iptext));
	}

	bool TcpSocket::connect(const NetworkEndPoint & ip_addr)
	{
		if (!Socket::closed()) {
			error_text("basic_tcp_socket::connect(): Already connected.");
			return false;
		}
		if (!ip_addr.valid()) {
			error_text("basic_tcp_socket::connect(): IP address no valid.");
			return false;
		}
		Socket::ip(ip_addr);
		if (!Socket::create()) {
			if (error_text().empty()) error_text("basic_tcp_socket::connect(): Failed to create socket.");
			return false;
		}
		if (!Socket::connect()) {
			error_text("basic_tcp_socket::connect(): Failed to connect.");
			return false;
		}
		return true;
	}

	bool TcpSocket::listen(const NetworkEndPoint & ip_addr, bool reuse_address, bool no_delay, int max_pending)
	{
		if (!Socket::closed()) {
			error_text("basic_tcp_socket::listen(): Already connected.");
			return false;
		}
		if (ip_addr.port() == 0) {
			error_text("basic_tcp_socket::listen(): Port not specified.");
			return false;
		}
		Socket::ip(ip_addr);
		if (!Socket::create()) {
			if (error_text().empty()) error_text("basic_tcp_socket::listen(): Failed to create socket.");
			return false;
		}
		Socket::reuseaddress(reuse_address);
		Socket::nodelay(no_delay);
		if (!Socket::bind()) {
			error_text("basic_tcp_socket::listen(): Failed to bind socket.");
			return false;
		}
		if (!Socket::listen(max_pending)) {
			error_text("basic_tcp_socket::listen(): Failed to start listening.");
			return false;
		}
		return true;
	}

	bool TcpSocket::accept(TcpSocket & acc)
	{
		return Socket::accept(acc);
	}

	void TcpSocket::close()
	{
		Socket::close();
	}

	bool TcpSocket::closed() const
	{
		return Socket::closed();
	}
}