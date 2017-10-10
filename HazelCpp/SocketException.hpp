#pragma once

#include <stdexcept>

namespace Hazel
{
	class SocketException : public std::runtime_error
	{
	public:
		SocketException();
		SocketException(const char *msg, long error);
		SocketException(std::exception &ex, long error);

		long GetError();

	private:
		long error;
	};
}