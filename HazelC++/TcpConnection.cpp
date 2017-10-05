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

			SetEndPoint(end_point);

			SetState(ConnectionState::Connected);
		};

		lock(socket_mutex, fn)
	}
}