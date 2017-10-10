#pragma once

#include "Hazel.hpp"
#include "NetworkConnection.hpp"
#include "TcpClient.hpp"

namespace Hazel
{
	class TcpConnection : public NetworkConnection, public TcpClient
	{
		TcpConnection(const TcpClient &client);

		void StartReceiving();
		void StartWaitingForHandshake(std::function<void(Bytes&)> &callback);

	public:
		TcpConnection(NetworkEndPoint end_point);

		void Connect(Bytes &bytes = Bytes(nullptr, -1), int timeout = 5000) override;
		void SendBytes(Bytes &bytes, SendOption send_option = SendOption::FragmentedReliable);

		void HeaderReadCallback(Bytes &bytes, std::function<void(Bytes&)> &callback);
		void BodyReadCallback(Bytes &bytes);
	};
}