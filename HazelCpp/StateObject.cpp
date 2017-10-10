#include "StateObject.hpp"

namespace Hazel
{
	StateObject::StateObject(int length, std::function<void(Bytes)> callback) : buffer(new byte[length](), length), total_bytes_received(0), callback(callback)
	{
	}
}