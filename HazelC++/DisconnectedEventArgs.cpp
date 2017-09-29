#include "DisconnectedEventArgs.hpp"

namespace Hazel
{
	DisconnectedEventArgs::DisconnectedEventArgs()
	{
		ObjectPool.AssignObjectFactory(CreateObject);
	}

	DisconnectedEventArgs::~DisconnectedEventArgs()
	{
		exception = HazelException();
		ObjectPool.PutObject(*this);
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
		return ObjectPool.GetObject();
	}

	HazelException DisconnectedEventArgs::GetException()
	{
		return exception;
	}
}