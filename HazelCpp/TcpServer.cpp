#include "TcpServer.hpp"

namespace Hazel
{
	TcpServer::TcpServer() : TcpSocket(), max_clients_(64), max_listen_pending_(64), TcpNoDelay(true), TcpReuseAddress(true)
	{
	}

	TcpServer::~TcpServer()
	{
		stop();
	}

	bool TcpServer::start()
	{
		return TcpSocket::listen(TcpSocket::ip(), TcpReuseAddress, TcpNoDelay, max_listen_pending_);
	}

	unsigned int TcpServer::max_clients() const throw()
	{
		return max_clients_;
	}

	void TcpServer::max_clients(unsigned int n) throw()
	{
		max_clients_ = n;
	}

	int TcpServer::max_clients_pending() const
	{
		return max_listen_pending_;
	}

	void TcpServer::max_clients_pending(int n)
	{
		max_listen_pending_ = n > 2 ? n : 2;
	}

	void TcpServer::stop()
	{
		if (!TcpSocket::closed())
		{
			max_clients_ = 0;
			TcpSocket::close();
		}
	}

	bool TcpServer::running() const
	{
		return !TcpSocket::closed();
	}

	TcpClient * TcpServer::accept()
	{
		if (TcpSocket::closed()) return nullptr;
		TcpClient client;
		if (TcpSocket::accept(client))
		{
			client.nodelay(true);
			return &client;
		}
		return nullptr;
	}

	bool TcpServer::connect(const NetworkEndPoint & ip_addr)
	{
		(void)ip_addr;
		return false;
	}

	bool TcpServer::connect()
	{
		return false;
	}
}