#pragma once

#include <functional>

#include "NewConnectionEventArgs.hpp"
#include "GenericFunction.hpp"
#include "Bytes.hpp"

namespace Hazel
{
	class ConnectionListener
	{
	public:
		GenericFunction<void, NewConnectionEventArgs> NewConnection;

		virtual void Start() = 0;

	protected:
		void InvokeNewConnection(Bytes bytes, NetworkConnection *connection);
	};
}