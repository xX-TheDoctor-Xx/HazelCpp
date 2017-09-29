#include "ConnectionListener.hpp"

namespace Hazel
{
	void ConnectionListener::InvokeNewConnection(Bytes bytes, NetworkConnection * connection)
	{
		NewConnectionEventArgs args;
		args.Set(bytes, connection);
		NewConnection.Call(args);
	}
}