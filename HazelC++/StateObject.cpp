#include "StateObject.hpp"

namespace Hazel
{
	StateObject::StateObject(int length, GenericFunction<void, Bytes> callback) : buffer(new byte[length](), length)
	{
		total_bytes_received = 0;
		this->callback = callback;
	}

	StateObject::~StateObject()
	{
		//should i do this?
		//delete[] buffer;
	}
}