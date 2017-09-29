#pragma once

#include "ObjectPool.hpp"
#include "HazelException.hpp"

namespace Hazel
{
	class DisconnectedEventArgs
	{
		HazelException exception;

	public:
		DisconnectedEventArgs();
		~DisconnectedEventArgs();

		static ObjectPool<DisconnectedEventArgs> ObjectPool;
		static DisconnectedEventArgs CreateObject();
		DisconnectedEventArgs &GetObject();

		HazelException GetException();

		void Set(const HazelException &ex);
	};
}