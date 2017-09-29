#pragma once

#include <functional>

namespace Hazel
{
	template<typename R, typename ...Args>
	class GenericFunction
	{
		std::function<R(Args...)> f;
	public:
		GenericFunction();

		void Set(std::function<R(Args...)> f);

		void Call(...);
	};
}