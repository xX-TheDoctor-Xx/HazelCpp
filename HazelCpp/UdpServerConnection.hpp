#pragma once

#include "UdpConnection.hpp"

namespace Hazel
{
	class UdpConnectionListener;

	class UdpServerConnection : public UdpConnection
	{
		UdpConnectionListener *listener;

		std::mutex state_mutex;

		void WriteBytesToConnection(Bytes &bytes) override;
	public:
		UdpServerConnection(UdpConnectionListener *listener, NetworkEndPoint &endpoint);

		UdpConnectionListener *GetListener();

		void Connect(Bytes &bytes = Bytes(nullptr, -1), int timeout = 5000) override;

	protected:
		void HandleDisconnect(HazelException &e = HazelException()) override;
		void Disconnect();
		void Close();
	};
}