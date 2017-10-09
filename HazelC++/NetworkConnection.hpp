#pragma once

#include "Hazel.hpp"
#include "NetworkEndPoint.hpp"
#include "ConnectionStatistics.hpp"
#include "DataReceivedEventArgs.hpp"
#include "DisconnectedEventArgs.hpp"
#include "ManualResetEvent.hpp"

namespace Hazel
{
	class NetworkConnection
	{
	public:
		NetworkConnection();

		std::function<void(DataReceivedEventArgs&)> DataReceived;
		std::function<void(DisconnectedEventArgs&)> Disconnected;

		NetworkEndPoint &GetEndpoint();
		void SetEndPoint(NetworkEndPoint &end_point);

		ConnectionStatistics &GetStatistics();
		ConnectionState GetState();
		void SetState(ConnectionState state);

		virtual void SendBytes(Bytes &bytes, SendOption sendOption = SendOption::None) = 0;
		virtual void Connect(Bytes &bytes = Bytes(nullptr, -1), int timeout = 5000) = 0;

		void InvokeDataReceived(Bytes &bytes, SendOption sendOption);
		void InvokeDisconnected(const HazelException &e = HazelException());

		bool WaitOnConnect(int timeout);

	private:
		NetworkEndPoint end_point;
		ConnectionStatistics Statistics;
		std::atomic<ConnectionState> State;

		std::atomic_bool connected;

		ManualResetEvent connect_wait_lock;

	protected:
		std::mutex socket_mutex;
	};
}