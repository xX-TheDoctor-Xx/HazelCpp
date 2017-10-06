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
		auto fn = [this]()
		{
			if (GetState() != ConnectionState::Connected)
			{
				throw HazelException("Could not send data as this Connection is not connected. Did you disconnect?");
			}

			//listener->SendData(bytes, GetEndpoint());
		};

		lock(state_mutex, fn)
	}

	void UdpServerConnection::Connect(Bytes bytes, int timeout)
	{
		throw HazelException("Cannot manually connect a UdpServerConnection, did you mean to use UdpClientConnection?");
	}

	void UdpServerConnection::HandleDisconnect(HazelException & e)
	{
		auto fn = [this]()
		{
			if (GetState() == ConnectionState::Connected)
				SetState(ConnectionState::Disconnecting);
		};

		lock(state_mutex, fn)

			if (e.ShouldHandle())
			{
				InvokeDisconnected(e);
				Close();
			}
	}

	void UdpServerConnection::Disconnect()
	{
		if (GetState() == ConnectionState::Connected)
			SendDisconnect();

		auto fn = [this]()
		{
			//listener->RemoveConnectionTo(GetEndpoint());
			SetState(ConnectionState::NotConnected);
		};

		lock(state_mutex, fn)
	}

	void UdpServerConnection::Close()
	{
		if (GetState() == ConnectionState::Connected)
			SendDisconnect();

		auto fn = [this]()
		{
			//listener->RemoveConnectionTo(GetEndpoint);
			SetState(ConnectionState::NotConnected);
		};

		lock(state_mutex, fn);
	}
}