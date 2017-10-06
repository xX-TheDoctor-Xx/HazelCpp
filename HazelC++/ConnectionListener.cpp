#include "ConnectionListener.hpp"

namespace Hazel
{
	ConnectionListener::ConnectionListener() : NewConnection(std::function<void(NewConnectionEventArgs)>())
	{
	}

	void ConnectionListener::InvokeNewConnection(Bytes bytes, NetworkConnection * connection)
	{
		NewConnectionEventArgs args = NewConnectionEventArgs::GetObject();
		args.Set(bytes, connection);
		NewConnection(args);
	}
}