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

		static ObjectPool<NewConnectionEventArgs> object_pool;
		static NewConnectionEventArgs CreateObject();

	public:

		NewConnectionEventArgs();
		~NewConnectionEventArgs();

		Bytes GetHandshakeData();
		NetworkConnection *GetConnection();
		static NewConnectionEventArgs &GetObject();

		void Set(Bytes handshake_data, NetworkConnection *connection);
	};
}