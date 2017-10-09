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
		auto fn = [this]()
		{
			if (pool.size() > 0)
				return pool.pop();
		};

		lock_mutex(pool_mutex, fn)

		return object_factory();
	}

	template<class T>
	void ObjectPool<T>::PutObject(T &object)
	{
		auto fn = [this]()
		{
			pool.push(object);
		};

		lock_mutex(pool_mutex, fn)
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