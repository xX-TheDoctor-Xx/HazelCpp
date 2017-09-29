#include "Hazel.hpp"

#undef lock

namespace Hazel
{
	bool Util::IsBigEndian()
	{
		return (htonl(47) == 47 ? true : false);
	}

	bool Util::SupportsIPv6()
	{
		return true;
	}

	void Util::BlockCopy(void * source, unsigned int source_offset, void * destination, unsigned int destination_offset, unsigned int count)
	{
		char *src = (char*)source;
		char *dst = (char*)destination;
		memcpy(dst + destination_offset, src + source_offset, count);
	}

	Lock::Lock(std::mutex & mutex) : mutex(mutex)
	{
		mutex.lock();
	}

	Lock::~Lock()
	{
		mutex.unlock();
	}

	Lock::operator bool() const
	{
		return false;
	}

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