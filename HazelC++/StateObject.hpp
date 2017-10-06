#pragma once

#include "Hazel.hpp"
#include "Bytes.hpp"

namespace Hazel
{
	class StateObject
	{
		Bytes buffer;
		int total_bytes_received;
		std::function<void(Bytes)> callback;

	public:
		StateObject(int length, std::function<void(Bytes)> callback);
	};
}