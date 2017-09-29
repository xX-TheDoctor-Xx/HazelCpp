#pragma once

#include "Hazel.hpp"
#include "GenericFunction.hpp"
#include "Bytes.hpp"

namespace Hazel
{
	class StateObject
	{
		Bytes buffer;
		int total_bytes_received;
		GenericFunction<void, Bytes> callback;

	public:
		StateObject(int length, GenericFunction<void, Bytes> callback);

		~StateObject();
	};
}