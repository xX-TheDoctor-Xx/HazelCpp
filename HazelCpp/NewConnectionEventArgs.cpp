#include "NewConnectionEventArgs.hpp"

namespace Hazel
{
	NewConnectionEventArgs NewConnectionEventArgs::CreateObject()
	{
		return NewConnectionEventArgs();
	}

	NewConnectionEventArgs::NewConnectionEventArgs()
	{
		object_pool.AssignObjectFactory(CreateObject);
	}

	NewConnectionEventArgs::~NewConnectionEventArgs()
	{
		object_pool.PutObject(*this);
	}

	Bytes &NewConnectionEventArgs::GetHandshakeData()
	{
		return handshake_data;
	}

	NetworkConnection * NewConnectionEventArgs::GetConnection()
	{
		return connection;
	}

	NewConnectionEventArgs & NewConnectionEventArgs::GetObject()
	{
		return object_pool.GetObject();
	}

	void NewConnectionEventArgs::Set(Bytes &handshake_data, NetworkConnection *connection)
	{
		this->handshake_data = handshake_data;
		this->connection = connection;
	}
}