#include "TcpConnection.hpp"

namespace Hazel
{
	TcpConnection::TcpConnection(Poco::Net::StreamSocket &soc)
	{
		//check if soc is Tcp which it should be

		lock(socket_mutex)
		{
			this->soc = &soc;
			this->soc->setNoDelay(true);

			SetState(ConnectionState::Connected);
		}
	}

	TcpConnection::TcpConnection(NetworkEndPoint end_point)
	{
		lock(socket_mutex)
		{
			if (GetState() != ConnectionState::NotConnected)
			{
				//throw new InvalidOperationException("Cannot connect as the Connection is already connected.");
			}

			SetEndPoint(end_point);

			this->soc = new Poco::Net::StreamSocket();
			this->soc->setNoDelay(true);

			SetState(ConnectionState::Connected);
		}
	}
}