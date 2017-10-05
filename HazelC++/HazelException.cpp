#include "HazelException.hpp"

namespace Hazel
{
	HazelException::HazelException() : std::runtime_error("And unexpected error occurred"), handle(false)
	{
	}

	HazelException::HazelException(const char * msg) : std::runtime_error(msg), handle(true)
	{
	}

	HazelException::HazelException(std::exception & ex) : std::runtime_error(ex.what()), handle(true)
	{
	}

	bool HazelException::ShouldHandle()
	{
		return handle;
	}
}