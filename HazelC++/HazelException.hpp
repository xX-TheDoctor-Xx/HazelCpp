#pragma once

#include <exception>

namespace Hazel
{
	class HazelException : public std::exception
	{
	public:
		HazelException() : handle(false)
		{
		}

		HazelException(const char *msg) : std::exception(msg), handle(true)
		{
		}

		HazelException(std::exception &ex) : std::exception(ex), handle(true)
		{
		}

		bool ShouldHandle()
		{
			return handle;
		}

	private:
		bool handle;
	};
}