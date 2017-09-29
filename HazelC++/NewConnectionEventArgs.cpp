#include "NewConnectionEventArgs.hpp"

namespace Hazel
{
	NewConnectionEventArgs NewConnectionEventArgs::CreateObject()
	{
		return NewConnectionEventArgs();
	}

	NewConnectionEventArgs::NewConnectionEventArgs()
	{
		ObjectPool.AssignObjectFactory(CreateObject);
	}

	NewConnectionEventArgs::~NewConnectionEventArgs()
	{
		ObjectPool.PutObject(*this);
	}

	Bytes NewConnectionEventArgs::GetHandshakeData()
	{
		return handshake_data;
	}

	NetworkConnection * NewConnectionEventArgs::GetConnection()
	{
		return connection;
	}

	NewConnectionEventArgs & NewConnectionEventArgs::GetObject()
	{
		return ObjectPool.GetObject();
	}

	void NewConnectionEventArgs::Set(Bytes handshake_data, NetworkConnection *connection)
	{
		this->handshake_data = handshake_data;
		this->connection = connection;
	}
}