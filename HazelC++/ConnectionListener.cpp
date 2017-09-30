#include "ConnectionListener.hpp"

namespace Hazel
{
	void ConnectionListener::InvokeNewConnection(Bytes bytes, NetworkConnection * connection)
	{
		NewConnectionEventArgs args = NewConnectionEventArgs::GetObject();
		args.Set(bytes, connection);
		NewConnection.Call(args);
	}
}