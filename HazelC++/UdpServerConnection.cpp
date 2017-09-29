#include "UdpServerConnection.hpp"
#include "UdpConnectionListener.hpp"

namespace Hazel
{
	UdpConnectionListener *UdpServerConnection::GetListener()
	{
		return listener;
	}

	UdpServerConnection::UdpServerConnection(UdpConnectionListener *listener, NetworkEndPoint endpoint)
	{
		this->listener = listener;
		SetEndPoint(endpoint);

		SetState(ConnectionState::Connected);
	}

	void UdpServerConnection::WriteBytesToConnection(Bytes bytes)
	{
		lock(state_mutex)
		{
			if (GetState() != ConnectionState::Connected)
			{
				//throw new InvalidOperationException("Could not send data as this Connection is not connected. Did you disconnect?");
			}

			//listener->SendData(bytes, GetEndpoint());
		}
	}

	void UdpServerConnection::Connect(Bytes bytes, int timeout)
	{
		//throw new HazelException("Cannot manually connect a UdpServerConnection, did you mean to use UdpClientConnection?");
	}

	void UdpServerConnection::HandleDisconnect(HazelException & e)
	{
		lock(state_mutex)
		{
			if (GetState() == ConnectionState::Connected)
				SetState(ConnectionState::Disconnecting);
		}

		if (e.ShouldHandle())
			InvokeDisconnected(e);
	}

	void UdpServerConnection::Disconnect()
	{
		if (GetState() == ConnectionState::Connected)
			SendDisconnect();

		lock(state_mutex)
		{
			//listener->RemoveConnectionTo(GetEndpoint());
			SetState(ConnectionState::NotConnected);
		}
	}
}