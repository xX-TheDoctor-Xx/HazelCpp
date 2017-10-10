#include "TcpClient.hpp"

namespace Hazel
{
	TcpClient::TcpClient(const TcpClient & client) : TcpSocket(client.socket_id())
	{
	}

	TcpClient::TcpClient() : TcpSocket()
	{
	}

	TcpClient::~TcpClient()
	{
	}

	void TcpClient::disconnect()
	{
		TcpSocket::close();
	}

	bool TcpClient::connect(const char * ip)
	{
		return TcpSocket::connect(ip);
	}

	bool TcpClient::connect(const NetworkEndPoint & ip)
	{
		return TcpSocket::connect(ip);
	}

	bool TcpClient::connected()
	{
		return !TcpSocket::closed();
	}

	bool TcpClient::closed()
	{
		return TcpSocket::closed();
	}

	long TcpClient::error()
	{
		return TcpSocket::error();
	}

	std::string TcpClient::error_text()
	{
		return TcpSocket::error_text();
	}

	bool TcpClient::listen(const NetworkEndPoint & ip_addr)
	{
		(void)ip_addr; 
		return false;
	}

	bool TcpClient::listen()
	{
		return false;
	}

	bool TcpClient::accept(TcpSocket & acc)
	{
		(void)acc; 
		return false;
	}

	bool TcpClient::create(SocketType type)
	{
		(void)type; 
		return false;
	}

	bool TcpClient::bind()
	{
		return false;
	}
}