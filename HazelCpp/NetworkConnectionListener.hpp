#pragma once

#include "Hazel.hpp"
#include "NetworkEndPoint.hpp"
#include "NewConnectionEventArgs.hpp"

namespace Hazel
{
	class NetworkConnectionListener
	{
	public:
		NetworkConnectionListener();
		NetworkEndPoint &GetEndPoint();

		std::function<void(NewConnectionEventArgs)> NewConnection;

		virtual void Start() = 0;

	protected:
		void SetEndPoint(NetworkEndPoint &end_point);

		void InvokeNewConnection(Bytes &bytes, NetworkConnection *connection);

		NetworkEndPoint end_point;
	};
}