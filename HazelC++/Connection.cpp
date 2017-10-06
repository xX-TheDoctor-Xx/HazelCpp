#include "Connection.hpp"
#include "Stopwatch.hpp"

namespace Hazel
{
	NetworkEndPoint Connection::GetEndpoint()
	{
		return end_point;
	}

	ConnectionStatistics &Connection::GetStatistics()
	{
		return Statistics;
	}

	ConnectionState Connection::GetState()
	{
		return State.load();
	}

	Connection::Connection() : DataReceived(std::function<void(DataReceivedEventArgs&)>()), Disconnected(std::function<void(DisconnectedEventArgs&)>())
	{
		State = ConnectionState::NotConnected;
	}

	void Connection::SetState(ConnectionState state)
	{
		State = state;
		
		if (state == ConnectionState::Connected)
			connected = true;
		else
			connected = false;
	}

	void Connection::InvokeDataReceived(Bytes bytes, SendOption sendOption)
	{
		DataReceivedEventArgs args = DataReceivedEventArgs::GetObject();
		args.Set(bytes, sendOption);
		DataReceived(args);
	}

	void Connection::InvokeDisconnected(const HazelException &e)
	{
		DisconnectedEventArgs args = DisconnectedEventArgs::GetObject();
		args.Set(e);
		Disconnected(args);
	}

	bool Connection::WaitOnConnect(int timeout)
	{
		Stopwatch stopwatch;
		stopwatch.Start();
		while (stopwatch.GetElapsedMilliseconds() < timeout && !connected);
		return connected;
	}

	void Connection::SetEndPoint(NetworkEndPoint end_point)
	{
		this->end_point = end_point;
	}
}