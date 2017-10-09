#include "TcpConnection.hpp"

namespace Hazel
{
	TcpConnection::TcpConnection(NetworkEndPoint end_point)
	{
		auto fn = [this, end_point]()
		{
			if (GetState() != ConnectionState::NotConnected)
			{
				//throw new InvalidOperationException("Cannot connect as the Connection is already connected.");
			}

			SetEndPoint(const_cast<NetworkEndPoint&>(end_point));

			SetState(ConnectionState::Connected);
		};

		lock_mutex(socket_mutex, fn)
	}
}