#pragma once

#include "ObjectPool.hpp"
#include "NetworkConnection.hpp"
#include "Bytes.hpp"

namespace Hazel
{
	class NewConnectionEventArgs
	{
		Bytes handshake_data;
		NetworkConnection *connection;
	public:
		static ObjectPool<NewConnectionEventArgs> ObjectPool;
		static NewConnectionEventArgs CreateObject();

		NewConnectionEventArgs();
		~NewConnectionEventArgs();

		Bytes GetHandshakeData();
		NetworkConnection *GetConnection();
		NewConnectionEventArgs &GetObject();

		void Set(Bytes handshake_data, NetworkConnection *connection);
	};
}