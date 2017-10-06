#include "Hazel.hpp"

#undef lock

namespace Hazel
{
	bool HazelInit::Initialize()
	{
	#ifdef _WIN32
		WSADATA data;
		return (initialized = WSAStartup(MAKEWORD(2, 2), &data) > 0);
	#else
		return true;
	#endif
	}

	void HazelInit::Cleanup()
	{
	#ifdef _WIN32
		if (initialized)
			initialized = !(WSACleanup() == 0);
	#endif
	}
}