#pragma once

#include <functional>

#include "NewConnectionEventArgs.hpp"
#include "Bytes.hpp"

namespace Hazel
{
	class ConnectionListener
	{
	public:
		ConnectionListener();
		std::function<void(NewConnectionEventArgs)> NewConnection;

		virtual void Start() = 0;

	protected:
		void InvokeNewConnection(Bytes bytes, NetworkConnection *connection);
	};
}