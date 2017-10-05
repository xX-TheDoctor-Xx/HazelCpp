#pragma once

#include <functional>

#include "DataReceivedEventArgs.hpp"
#include "DisconnectedEventArgs.hpp"
#include "NetworkEndPoint.hpp"
#include "ConnectionStatistics.hpp"
#include "ManualResetEvent.hpp"
#include "HazelException.hpp"
#include "GenericFunction.hpp"
#include "Bytes.hpp"

namespace Hazel
{
	class Connection
	{
	public:
		Connection();

		GenericFunction<DataReceivedEventArgs> DataReceived;
		GenericFunction<DisconnectedEventArgs> Disconnected;

		NetworkEndPoint GetEndpoint();
		void SetEndPoint(NetworkEndPoint end_point);

		ConnectionStatistics &GetStatistics();
		ConnectionState GetState();
		void SetState(ConnectionState state);

		virtual void SendBytes(Bytes bytes, SendOption sendOption = SendOption::None) = 0;
		virtual void Connect(Bytes bytes = Bytes(nullptr, -1), int timeout = 5000) = 0;

		void InvokeDataReceived(Bytes bytes, SendOption sendOption);
		void InvokeDisconnected(const HazelException &e = HazelException());

		bool WaitOnConnect(int timeout);

	private:
		NetworkEndPoint end_point;
		ConnectionStatistics Statistics;
		std::atomic<ConnectionState> State;

		ManualResetEvent connection_wait_lock;
	};
}