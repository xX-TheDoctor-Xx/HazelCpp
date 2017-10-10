#pragma once

#include <stdexcept>

namespace Hazel
{
	class HazelException : public std::runtime_error
	{
	public:
		HazelException();
		HazelException(const char *msg);
		HazelException(std::exception &ex);

		bool ShouldHandle();

	private:
		bool handle;
	};
}