#pragma once

#include "ObjectPool.hpp"
#include "HazelException.hpp"

namespace Hazel
{
	class DisconnectedEventArgs
	{
		HazelException exception;
		
		static ObjectPool<DisconnectedEventArgs> object_pool;
		static DisconnectedEventArgs CreateObject();

	public:
		DisconnectedEventArgs();
		~DisconnectedEventArgs();

		static DisconnectedEventArgs &GetObject();

		HazelException GetException();

		void Set(const HazelException &ex);
	};
}