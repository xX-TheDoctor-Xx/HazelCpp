#include "DisconnectedEventArgs.hpp"

namespace Hazel
{
	DisconnectedEventArgs::DisconnectedEventArgs()
	{
		object_pool.AssignObjectFactory(CreateObject);
	}

	DisconnectedEventArgs::~DisconnectedEventArgs()
	{
		exception = HazelException();
		object_pool.PutObject(*this);
	}

	void DisconnectedEventArgs::Set(const HazelException &ex)
	{
		exception = ex;
	}

	DisconnectedEventArgs DisconnectedEventArgs::CreateObject()
	{
		return DisconnectedEventArgs();
	}

	DisconnectedEventArgs & DisconnectedEventArgs::GetObject()
	{
		return object_pool.GetObject();
	}

	HazelException DisconnectedEventArgs::GetException()
	{
		return exception;
	}
}