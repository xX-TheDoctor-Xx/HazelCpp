#pragma once

#include "Hazel.hpp"

#include <queue>
#include <functional>

namespace Hazel
{
	template<class T>
	class ObjectPool
	{
		std::queue<T> pool;
		std::function<T()> object_factory;
		std::mutex pool_mutex;

	public:
		ObjectPool();
		ObjectPool(const ObjectPool &other);

		void AssignObjectFactory(const std::function<T()> &object_factory);
		T &GetObject();
		void PutObject(T &object);
	};
}