#include "SocketException.hpp"

namespace Hazel
{
	SocketException::SocketException() : std::runtime_error("And unexpected error occurred")
	{
	}

	SocketException::SocketException(const char * msg, long error) : std::runtime_error(msg), error(error)
	{
	}

	SocketException::SocketException(std::exception & ex, long error) : std::runtime_error(ex.what()), error(error)
	{
	}

	long SocketException::GetError()
	{
		return error;
	}
}