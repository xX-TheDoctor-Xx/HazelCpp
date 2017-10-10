#include "NetworkConnection.hpp"
#include "Stopwatch.hpp"

namespace Hazel
{
	NetworkConnection::NetworkConnection() : DataReceived(std::function<void(DataReceivedEventArgs&)>()), Disconnected(std::function<void(DisconnectedEventArgs&)>()), connect_wait_lock(false)
	{
		State = ConnectionState::NotConnected;
	}

	NetworkEndPoint &NetworkConnection::GetEndpoint()
	{
		return end_point;
	}

	ConnectionStatistics &NetworkConnection::GetStatistics()
	{
		return Statistics;
	}

	ConnectionState NetworkConnection::GetState()
	{
		return State.load();
	}

	void NetworkConnection::SetState(ConnectionState state)
	{
		State = state;

		if (state == ConnectionState::Connected)
			connect_wait_lock.set();
		else
			connect_wait_lock.reset();
	}

	void NetworkConnection::InvokeDataReceived(Bytes &bytes, SendOption sendOption)
	{
		DataReceivedEventArgs args = DataReceivedEventArgs::GetObject();
		args.Set(bytes, sendOption);
		DataReceived(args);
	}

	void NetworkConnection::InvokeDisconnected(const HazelException &e)
	{
		DisconnectedEventArgs args = DisconnectedEventArgs::GetObject();
		args.Set(e);
		Disconnected(args);
	}

	bool NetworkConnection::WaitOnConnect(int timeout)
	{
		return connect_wait_lock.wait(timeout);
	}

	void NetworkConnection::SetEndPoint(NetworkEndPoint &end_point)
	{
		this->end_point = end_point;
	}
}