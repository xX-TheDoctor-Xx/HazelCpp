#include "Connection.hpp"

namespace Hazel
{
	NetworkEndPoint Connection::GetEndpoint()
	{
		return end_point;
	}

	ConnectionStatistics Connection::GetStatistics()
	{
		return Statistics;
	}

	ConnectionState Connection::GetState()
	{
		return State.load();
	}

	Connection::Connection()
	{
		State = ConnectionState::NotConnected;
	}

	void Connection::SetState(ConnectionState state)
	{
		State = state;
		
		if (state == ConnectionState::Connected)
			connectionWaitLock.Notify();
	}

	void Connection::InvokeDataReceived(Bytes bytes, SendOption sendOption)
	{
		DataReceivedEventArgs args;
		args.Set(bytes, sendOption);
		DataReceived.Call(args);
	}

	void Connection::InvokeDisconnected(const HazelException &e)
	{
		DisconnectedEventArgs args;
		args.Set(e);
		Disconnected.Call(args);
	}

	bool Connection::WaitOnConnect(int timeout)
	{
		return connectionWaitLock.Wait(timeout);
	}

	void Connection::SetEndPoint(NetworkEndPoint end_point)
	{
		this->end_point = end_point;
	}
}