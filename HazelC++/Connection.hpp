#pragma once

#include <functional>

#include "DataReceivedEventArgs.hpp"
#include "DisconnectedEventArgs.hpp"
#include "NetworkEndPoint.hpp"
#include "ConnectionStatistics.hpp"
#include "AutoResetEvent.hpp"
#include "HazelException.hpp"
#include "GenericFunction.hpp"
#include "Bytes.hpp"

#include <Poco/Net/SocketAddress.h>

namespace Hazel
{
	class Connection
	{
	public:
		GenericFunction<DataReceivedEventArgs> DataReceived;
		GenericFunction<DisconnectedEventArgs> Disconnected;

		NetworkEndPoint GetEndpoint();
		void SetEndPoint(NetworkEndPoint end_point);

		ConnectionStatistics GetStatistics();
		ConnectionState GetState();

		virtual void SendBytes(Bytes bytes, SendOption sendOption = SendOption::None) = 0;
		virtual void Connect(Bytes bytes = Bytes(nullptr, -1), int timeout = 5000) = 0;

	protected:
		Connection();

		NetworkEndPoint end_point;
		ConnectionStatistics Statistics;
		std::atomic<ConnectionState> State;

		void SetState(ConnectionState state);

		void InvokeDataReceived(Bytes bytes, SendOption sendOption);
		void InvokeDisconnected(const HazelException &e = HazelException());

		bool WaitOnConnect(int timeout);

	private:
		AutoResetEvent connectionWaitLock;
	};
}