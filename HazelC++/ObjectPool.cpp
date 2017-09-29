#include "ObjectPool.hpp"

namespace Hazel
{
	template<class T>
	ObjectPool<T>::ObjectPool()
	{
	}

	template<class T>
	T &ObjectPool<T>::GetObject()
	{
		lock(pool_mutex)
		{
			if (pool.size() > 0)
				return pool.pop();
		}

		return object_factory();
	}

	template<class T>
	void ObjectPool<T>::PutObject(T &object)
	{
		lock(pool_mutex)
		{
			pool.push(object);
		}
	}

	template<class T>
	ObjectPool<T>::ObjectPool(const ObjectPool &other)
	{
		pool = other.pool;
		object_factory = other.object_factory;
		pool_mutex = other.pool_mutex;
	}

	template<class T>
	void ObjectPool<T>::AssignObjectFactory(const std::function<T()> &object_factory)
	{
		this->object_factory = object_factory;
	}
}