#include "NetworkConnectionListener.hpp"

namespace Hazel
{

	NetworkConnectionListener::NetworkConnectionListener() : NewConnection(std::function<void(NewConnectionEventArgs)>())
	{
	}

	NetworkEndPoint &NetworkConnectionListener::GetEndPoint()
	{
		return end_point;
	}

	void NetworkConnectionListener::SetEndPoint(NetworkEndPoint &end_point)
	{
		this->end_point = end_point;
	}

	void NetworkConnectionListener::InvokeNewConnection(Bytes &bytes, NetworkConnection * connection)
	{
		NewConnectionEventArgs args = NewConnectionEventArgs::GetObject();
		args.Set(bytes, connection);
		NewConnection(args);
	}
}